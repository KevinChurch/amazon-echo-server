#include "bad_request_handler.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "reply.h"
#include "request.h"

BadRequestHandler* BadRequestHandler::create(const NginxConfig& config,
                                         const std::string& root_path) {
  return new BadRequestHandler;
}

std::unique_ptr<Reply> BadRequestHandler::HandleRequest(const Request& request) {
  BOOST_LOG_SEV(my_logger::get(), INFO) << "\nBadRequestHandler::HandleRequest";
  BOOST_LOG_SEV(my_logger::get(), INFO) << "::ResponseMetrics:: Handler Name: BadRequestHandler";
  BOOST_LOG_SEV(my_logger::get(), INFO) << "::ResponseMetrics:: Request Path: " << request.uri();

  std::unique_ptr<Reply> reply_ptr(new Reply());
  std::string body = "400 BAD REQUEST\r\n";
  BOOST_LOG_SEV(my_logger::get(), INFO) << "::ResponseMetrics:: Response Code: 400";
  reply_ptr->SetStatus(400);
  reply_ptr->SetHeader("Content-Type", "text/plain");
  reply_ptr->SetBody(body);
  return reply_ptr;
}
