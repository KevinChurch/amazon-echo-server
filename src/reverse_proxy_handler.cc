#include "reverse_proxy_handler.h"

ReverseProxyHandler* ReverseProxyHandler::create(const NginxConfig& config,
                                     const std::string& root_path) {
  ReverseProxyHandler* handler = new ReverseProxyHandler;
  handler->Init(config, root_path);
  return handler;
}

bool ReverseProxyHandler::Init(const NginxConfig& config,
                         const std::string& root_path) {
  this->root_path = root_path;
  this->uri_prefix = config.Find("location");
  this->host_name = config.Find("host");
  this->port_name = config.Find("port");

  // remove http:// or https:// from the host name if it exists
  RemoveHTTPProtocol(this->host_name);

  return true;
}

/** This function modifies the given request
 * connects to the desired HTTP server
 * sends the modified request to the server
 * reads through the socket for the response
 * sets the reply_ptr status/headers/body based off of the response
 * NOTE: this has issues with connecting to https servers (e.g. port 443)
 */
std::unique_ptr<Reply> ReverseProxyHandler::HandleRequest(const Request& request) {
  BOOST_LOG_SEV(my_logger::get(), INFO) << "ReverseProxyHandler::HandleRequest";
  BOOST_LOG_SEV(my_logger::get(), TRACE) << "[Reverse Proxy] root path: " << this->root_path
    << ", location: " << this->uri_prefix
    << ", host: " << this->host_name
    << ", port: " << this->port_name;

  std::unique_ptr<Reply> reply_ptr(new Reply());
  // remove the uri prefix from the request and issue it to a new response
  std::string resp_string = request.method() + " " + GetPath(request.uri()) + " HTTP/1.1\r\nHost: "
    + this->host_name + "\r\nAccept-Encoding: utf-8\r\nConnection: close\r\n\r\n";
  boost::asio::streambuf resp;
  std::ostream resp_stream(&resp);
  resp_stream << resp_string;

  // connect to the HTTP server
  boost::asio::io_service io_service;
  boost::asio::ip::tcp::socket socket(io_service);
  boost::system::error_code error_code;
  boost::asio::ip::tcp::resolver::iterator iter = ResolveHost(error_code);
  if (error_code) {
    return ReturnError(std::move(reply_ptr), error_code);
  }

  boost::asio::connect(socket, iter, error_code);
  if (error_code) {
    return ReturnError(std::move(reply_ptr), error_code);
  }
  boost::asio::write(socket, resp, error_code);
  if (error_code) {
    return ReturnError(std::move(reply_ptr), error_code);
  }

  boost::asio::streambuf read_buf;
  size_t bytes_transferred = boost::asio::read(socket, read_buf, error_code);
  if (error_code != boost::asio::error::eof) {
    return ReturnError(std::move(reply_ptr), error_code);
  }
  std::string response(boost::asio::buffers_begin(read_buf.data()),
    boost::asio::buffers_begin(read_buf.data()) + bytes_transferred);

  RedirectHTMLMaps(response);

  // find the start of the body
  // NOTE: this is under the assumption that the response contains carriage returns
  size_t start = response.find("\r\n\r\n");

  int status = GetStatus(response);
  reply_ptr->SetStatus(status);

  // NOTE: it seems like www.ucla.edu is some exception where there will be some random
  // hexadecimal string (e.g. 00637e) at the top of the body, and '0' at the end of it
  // Maybe it is worth hardcoding a fix?
  std::string body = GetBody(response, start);
  reply_ptr->SetBody(body);
  
  std::map<std::string, std::string> m_headers = GetHeaders(response, start);
  reply_ptr = SetHeaders(std::move(reply_ptr), m_headers, request);

  return reply_ptr;
}

void ReverseProxyHandler::RemoveHTTPProtocol(std::string& hostname) {
  // perform on both http:// and https://
  size_t pos = hostname.find("http://");
  if (pos != std::string::npos) {
    hostname = hostname.substr(pos + strlen("http://"));
  } else {
    pos = hostname.find("https://");
    if (pos != std::string::npos) {
      hostname = hostname.substr(pos + strlen("https://"));
      // in case it redirects to https, change the port
      this->port_name = "443";
    }
  }
}

std::string ReverseProxyHandler::GetPath(std::string url) {
  // We assume the url has the prefix declared in the ReverseProxyHandler
  int prefix_len = this->uri_prefix.length();
  std::string path = url.substr(prefix_len);

  if (path == "") {
    path = "/";
  }
  return path;
}

int ReverseProxyHandler::GetStatus(const std::string& resp) {
  // get the positions of the first and second space
  // this is under the assumption that the http version is always first in an http response
  size_t start = resp.find(" ");
  size_t end = resp.find(" ", start + 1);
  if (end != std::string::npos) {
    std::string str(resp.begin() + start + 1, resp.begin() + end);
    return stoi(str);
  }
  // bad request
  return 400;
}

std::map<std::string, std::string> ReverseProxyHandler::GetHeaders(const std::string& resp, const size_t& start) {
  std::vector<std::string> result;
  std::map<std::string, std::string> headers;
  std::string str = resp.substr(0, start);
  boost::split(result, str, boost::is_any_of("\r\n"));
  for (int i = 0; i < result.size(); i++) {
    size_t pos = result[i].find(":");
    if (pos != std::string::npos) {
      // 2 represents the number of bytes in ": "
      // e.g. result[i] = "Content-Type: text/plain" -> headers[Content-Type] = text/plain
      headers[result[i].substr(0, pos)] = result[i].substr(pos + 2);
    }
  }
  return headers;
}

std::string ReverseProxyHandler::GetBody(const std::string& resp, const size_t& start) {
  if (start != std::string::npos) {
    return resp.substr(start + strlen("\r\n\r\n"));
  }
  return "";
}

std::unique_ptr<Reply> ReverseProxyHandler::SetHeaders(std::unique_ptr<Reply> rep,
                                                       const std::map<std::string, std::string>& headers,
                                                       const Request& request) {
  for (auto it = headers.begin(); it != headers.end(); it++) {
    // it->first is the key (header name)
    // it->second is the value
    if (it->first == "Content-Type") {
      /** There's some weird bug that occurs if this accepts all headers (i.e. no if statement)
       * Output when typing localhost:8080/ucla/ in the browser (just like w/ the static handler in the local browser):
       *    Invalid method request!
       *    HTTP Version not support!
       *    Segmentation fault (core dumped)
       */
      rep->SetHeader(it->first, it->second);
    } else if (it->first == "Location") {
      BOOST_LOG_SEV(my_logger::get(), INFO) << "Redirect " << this->host_name  << GetPath(request.uri())
                                            << " to " << it->second;

      std::string path = GetNewHostAndPath(it->second);
      auto req = request;
      req.set_uri(path);

      return HandleRequest(req);
    }
  }
  return rep;
}

std::string ReverseProxyHandler::GetNewHostAndPath(const std::string& new_location) {
  this->host_name = new_location;
  RemoveHTTPProtocol(this->host_name);

  // '/' is not a valid character in a host name, so that is the start of the path
  size_t pos = this->host_name.find("/");
  std::string suffix;
  if (pos != std::string::npos) {
    suffix = this->host_name.substr(pos);
    this->host_name = this->host_name.substr(0, pos);
  }
  // return the path
  return this->uri_prefix + suffix;
}

void ReverseProxyHandler::RedirectHTMLMaps(std::string& resp) {
  // redirect mappings in html files to /uri_prefix/map
  boost::replace_all(resp, "href=\"/", "href=\"" + this->uri_prefix + "/");
  boost::replace_all(resp, "src=\"/", "src=\"" + this->uri_prefix + "/");

  // css style
  boost::replace_all(resp, "url(/", "url(" + this->uri_prefix + "/");
  boost::replace_all(resp, "src='", "src='" + this->uri_prefix + "/");
}

boost::asio::ip::tcp::resolver::iterator ReverseProxyHandler::ResolveHost(boost::system::error_code& ec) {
  boost::asio::io_service io_service;                    
  boost::asio::ip::tcp::resolver::query query(this->host_name, this->port_name,
    boost::asio::ip::tcp::resolver::query::numeric_service);
  boost::asio::ip::tcp::resolver resolver(io_service);
  boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query, ec);
  return iter;
}

std::unique_ptr<Reply> ReverseProxyHandler::ReturnError(std::unique_ptr<Reply> rep,
                                                        const boost::system::error_code& ec) {
  // Internal Server Error
  std::cerr << "Error #" << ec.value() << ": " << ec.message();
  BOOST_LOG_SEV(my_logger::get(), ERROR) << "Error #" << ec.value() << ": " << ec.message();
  rep->SetStatus(500);
  rep->SetHeader("Content-Type", "text/plain");
  rep->SetBody("500 Internal Server Error\r\n");
  return rep;
}
