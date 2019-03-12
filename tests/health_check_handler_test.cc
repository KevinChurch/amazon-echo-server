#include "health_check_handler.h"
#include "config_parser.h"
#include "gtest/gtest.h"
#include "handler.h"
#include "reply.h"
#include "request.h"

class HealthCheckHandlerTest : public ::testing::Test {
 protected:
  NginxConfig config;
  HealthCheckHandler* handler = HealthCheckHandler::create(config, "./");
  std::string request_string = "GET /health HTTP/1.1\r\n\r\n";
  std::unique_ptr<Request> request_ = Request::ParseRequest(request_string);
};

TEST_F(HealthCheckHandlerTest, HandleRequest) {
  std::unique_ptr<Reply> reply = handler->HandleRequest(*request_);

  EXPECT_EQ(reply->ToString(),
            "\
HTTP/1.1 200 OK\r\n\
\r\n\
OK\r\n");
}