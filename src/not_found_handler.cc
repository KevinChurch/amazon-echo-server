#include "not_found_handler.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "reply.h"
#include "request.h"

NotFoundHandler* NotFoundHandler::create(const NginxConfig& config,
                                         const std::string& root_path) {
  return new NotFoundHandler;
}

std::unique_ptr<Reply> NotFoundHandler::HandleRequest(const Request& request) {
  BOOST_LOG_SEV(my_logger::get(), INFO) << "\nNotFoundHandler::HandleRequest";
  BOOST_LOG_SEV(my_logger::get(), INFO) << "::ResponseMetrics:: Handler Name: NotFoundHandler";
  BOOST_LOG_SEV(my_logger::get(), INFO) << "::ResponseMetrics:: Request Path: " << request.uri();

  std::unique_ptr<Reply> reply_ptr(new Reply());
  std::string body = "404 NOT FOUND\r\n";
  BOOST_LOG_SEV(my_logger::get(), INFO) << "::ResponseMetrics:: Response Code: 404";
  reply_ptr->SetStatus(404);
  reply_ptr->SetHeader("Content-Type", "text/plain");
  reply_ptr->SetBody(body);
  return reply_ptr;
}
