#include "status_handler.h"
#include "reply.h"
#include "request.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>

StatusHandler* StatusHandler::create(const NginxConfig& config, const std::string& root_path) {
  return new StatusHandler;
}

std::unique_ptr<Reply> StatusHandler::HandleRequest(const Request& request) {
  std::cout << "\nStatusHandler::HandleRequest" << std::endl;
  BOOST_LOG_SEV(my_logger::get(), INFO) << "\nStatusHandler::HandleRequest";

  std::unique_ptr<Reply> reply_ptr(new Reply());
  reply_ptr->SetStatus(200);
  reply_ptr->SetHeader("Content-Type", "text/plain");
  std::string body_str = "";
  body_str += requestInfoToString();
  body_str += handlersToString();
  reply_ptr->SetBody(body_str);
  return reply_ptr;
}

void StatusHandler::setRequestInfo(std::map<std::string, std::map<int, int>> * request_info) {
  this->request_info_map = request_info;
}

void StatusHandler::setHandlers(std::vector<NginxConfig*> handlers) {
  this->handlers = handlers;
}

std::string StatusHandler::requestInfoToString() {
  std::string request_info = "Status of web server:\n(URL - RESPONS_CODE: NUMBER_OF_REQUESTS)\n";
  int total_count = 0;

  for (auto const& x : (*(this->request_info_map)))
  {
    request_info = request_info + x.first + " - ";
    for (auto const& y: x.second)
    {
      request_info = request_info + std::to_string(y.first) + ": " + std::to_string(y.second) + " requests\n";
      total_count += y.second;
    }
  }

  request_info = request_info + "total number of requests: " + std::to_string(total_count) + "\n\n";

  return request_info;
}

std::string StatusHandler::handlersToString() {
  std::string handler_str = "Existing Handlers:\n(HANDLER - URL_PREFIX)\n";

  for (std::vector<NginxConfig*>::iterator iter = handlers.begin(); iter != handlers.end(); iter++){
    std::string handler_name = (*iter)->Find("name");
    std::string url_prefix = (*iter)->Find("location");
    handler_str = handler_str + handler_name + " - " + url_prefix + "\n";
  }

  return handler_str;
};