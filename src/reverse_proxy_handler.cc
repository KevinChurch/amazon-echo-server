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

  BOOST_LOG_SEV(my_logger::get(), TRACE) << "[Reverse Proxy] root path: " << this->root_path
    << ", location: " << this->uri_prefix
    << ", host: " << this->host_name
    << ", port: " << this->port_name;

  return true;
}

std::unique_ptr<Reply> ReverseProxyHandler::HandleRequest(const Request& request) {
  BOOST_LOG_SEV(my_logger::get(), INFO) << "ReverseProxyHandler::HandleRequest";

  std::unique_ptr<Reply> reply_ptr(new Reply());
  // remove the uri prefix from the request and issue it to a new response
  std::string resp_string = request.method() + " " + GetPath(request.uri()) + " HTTP/1.1\r\nHost: "
    + this->host_name + "\r\nConnection: close\r\n\r\n";
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

  // redirect mappings in html files to /uri_prefix/map
  boost::replace_all(response, "href=\"/", "href=\"" + this->uri_prefix + "/");
  boost::replace_all(response, "src=\"/", "src=\"" + this->uri_prefix + "/");

  // css style
  boost::replace_all(response, "url(/", "url(" + this->uri_prefix + "/");
  boost::replace_all(response, "src='", "src='" + this->uri_prefix + "/");

  // find the start of the body
  // NOTE: this is under the assumption that the response contains carriage returns
  size_t start = response.find("\r\n\r\n");

  int status = GetStatus(response);
  reply_ptr->SetStatus(status);

  // TODO (leilaomar or johnstucky): fix the weird address and zero character at the
  // start and end of the body respectively
  std::string body = GetBody(response, start);
  reply_ptr->SetBody(body);
  
  std::map<std::string, std::string> m_headers = GetHeaders(response, start);
  for (auto it = m_headers.begin(); it != m_headers.end(); it++) {
    // it->first is the key (header name)
    // it->second is the value
    if (it->first == "Content-Type") {
      /** There's some weird bug that occurs if this accepts all headers (i.e. no if statement)
       * Output when typing localhost:8080/ucla/ in the browser (just like w/ the static handler in the local browser):
       *    Invalid method request!
       *    HTTP Version not support!
       *    Segmentation fault (core dumped)
       */
      reply_ptr->SetHeader(it->first, it->second);
    } else if (it->first == "Location") {
      // TODO (leilaomar or johnstucky): fix redirect w/ status 301/302 by modifying the location
      BOOST_LOG_SEV(my_logger::get(), INFO) << "Redirect " << request.uri() << " to " << it->second;
      auto req = request;
      req.set_uri(it->second);
      return HandleRequest(req);
    }
  }

  return reply_ptr;
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
