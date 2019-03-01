#include "status_handler.h"
#include "config_parser.h"
#include "gtest/gtest.h"
#include "handler.h"
#include "reply.h"
#include "request.h"

class StatusHandlerTest : public ::testing::Test {
 protected:
  StatusHandler* handler;
  Request request;
  NginxConfig config;
  NginxConfig* handler_config_ptr;
  NginxConfigParser config_parser;
  std::map<std::string, std::map<int, int>>* request_map =
      new std::map<std::string, std::map<int, int>>();
};

TEST_F(StatusHandlerTest, HandleRequest) {
  // Parse example config
  config_parser.Parse("new_example_config", &config);

  // Create Request
  request.ParseRequest("GET /status HTTP/1.1\r\n\r\n");

  // Find Not Found Handler in config
  std::vector<NginxConfig*> handlers = config.FindBlocks("handler");
  for (std::vector<NginxConfig*>::iterator iter = handlers.begin();
       iter != handlers.end(); iter++) {
    if ((*iter)->Find("name") == "status") {
      handler_config_ptr = *iter;
      break;
    }
  }

  // Create Handler
  handler = StatusHandler::create(config, "./");
  handler->setRequestInfo(request_map);
  handler->setHandlers(handlers);

  // Create Reply
  std::unique_ptr<Reply> reply = handler->HandleRequest(request);

  // Test Handler's reply
  EXPECT_EQ(reply->ToString(),
            "\
HTTP/1.1 200 OK\r\n\
Content-Type: text/plain\r\n\
\r\n\
Status of web server:\n\
(URL - RESPONS_CODE: NUMBER_OF_REQUESTS)\n\
total number of requests: 0\n\
\n\
Existing Handlers:\n\
(HANDLER - URL_PREFIX)\n\
static - /static\n\
static - /static2\n\
echo - /echo\n\
not_found - /\n\
status - /status\n\
reverse_proxy - /local\n");
}