#include "health_check_handler.h"
#include "reply.h"
#include "request.h"

HealthCheckHandler* HealthCheckHandler::create(const NginxConfig& config,
                                   const std::string& root_path) {
  return new HealthCheckHandler;
}

std::unique_ptr<Reply> HealthCheckHandler::HandleRequest(const Request& request) {
  BOOST_LOG_SEV(my_logger::get(), INFO) << "\n::ResponseMetrics::HealthCheckHandler::HandleRequest";
  BOOST_LOG_SEV(my_logger::get(), INFO) << "::ResponseMetrics:: Request Path: " << request.uri();

  std::unique_ptr<Reply> reply_ptr(new Reply());
  std::string body = "OK\r\n";
  BOOST_LOG_SEV(my_logger::get(), INFO) << "::ResponseMetrics:: Response Code: 200";
  reply_ptr->SetStatus(200);
  reply_ptr->SetBody(body);
  return reply_ptr;
}
