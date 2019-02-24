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

  return true;
}

std::unique_ptr<Reply> ReverseProxyHandler::HandleRequest(const Request& request) {
  std::cout << "\nReverseProxyHandler::HandleRequest" << std::endl;
  BOOST_LOG_SEV(my_logger::get(), INFO) << "\nReverseProxyHandler::HandleRequest";

  std::unique_ptr<Reply> reply_ptr(new Reply());

  return reply_ptr;
}
