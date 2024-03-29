#include "echo_handler.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "reply.h"
#include "request.h"

/**
    config and root_path is not used but included to comply with interface.
*/
EchoHandler* EchoHandler::create(const NginxConfig& config,
                                 const std::string& root_path) {
  return new EchoHandler;
}

/**
    HandleRequest echoes request by setting the original request as the response body.
*/
std::unique_ptr<Reply> EchoHandler::HandleRequest(const Request& request) {
  BOOST_LOG_SEV(my_logger::get(), INFO) << "\nEchoHandler::HandleRequest";
  BOOST_LOG_SEV(my_logger::get(), INFO) << "::ResponseMetrics:: Handler Name: EchoHandler";
  BOOST_LOG_SEV(my_logger::get(), INFO) << "::ResponseMetrics:: Request Path: " << request.uri();

  std::unique_ptr<Reply> reply_ptr(new Reply());
  BOOST_LOG_SEV(my_logger::get(), INFO) << "::ResponseMetrics:: Response Code: 200";
  reply_ptr->SetStatus(200);
  reply_ptr->SetHeader("Content-Type", "text/plain");
  reply_ptr->SetBody(request.original_request());
  return reply_ptr;
}
