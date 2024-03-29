#include "sleep_handler.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "reply.h"
#include "request.h"

SleepHandler* SleepHandler::create(const NginxConfig& config,
                                   const std::string& root_path) {
  return new SleepHandler;
}

std::unique_ptr<Reply> SleepHandler::HandleRequest(const Request& request) {
  BOOST_LOG_SEV(my_logger::get(), INFO) << "\nSleepHandler::HandleRequest";
  BOOST_LOG_SEV(my_logger::get(), INFO) << "::ResponseMetrics:: Handler Name: SleepHandler";
  BOOST_LOG_SEV(my_logger::get(), INFO) << "::ResponseMetrics:: Request Path: " << request.uri();

  std::this_thread::sleep_for(std::chrono::seconds(5));

  std::unique_ptr<Reply> reply_ptr(new Reply());
  std::string body = "Sleping 5 seconds...\r\n";
  BOOST_LOG_SEV(my_logger::get(), INFO) << "::ResponseMetrics:: Response Code: 200";
  reply_ptr->SetStatus(200);
  reply_ptr->SetHeader("Content-Type", "text/plain");
  reply_ptr->SetBody(body);
  return reply_ptr;
}
