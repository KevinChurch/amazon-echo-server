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

  // TODO (leilaomar): change header type/refactor GetContentType to base class and use it here
  reply_ptr->SetStatus(200);
  reply_ptr->SetHeader("Content-Type", "text/plain");
  // TODO (leilaomar): get headers from response and only use the body for SetBody
  reply_ptr->SetBody(response);
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
