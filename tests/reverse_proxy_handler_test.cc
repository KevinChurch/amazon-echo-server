#include "reverse_proxy_handler.h"
#include "config_parser.h"
#include "gtest/gtest.h"
#include "handler.h"
#include "reply.h"
#include "request.h"

class ReverseProxyHandlerTest : public ::testing::Test {
 protected:
  ReverseProxyHandler* handler;
  Request request;
  NginxConfig config;
  NginxConfig* handler_config_ptr;
  NginxConfigParser config_parser;
};

TEST_F(ReverseProxyHandlerTest, HandleRequestNoConnection) {
  // Parse example config
  config_parser.Parse("new_example_config", &config);

  // Create Request
  request.ParseRequest("GET /local/static/index.html HTTP/1.1\r\n\r\n");

  // Find Reverse Proxy Handler in config
  std::vector<NginxConfig*> handlers = config.FindBlocks("handler");
  for (std::vector<NginxConfig*>::iterator iter = handlers.begin();
       iter != handlers.end(); iter++) {
    if ((*iter)->Find("name") == "reverse_proxy") {
      handler_config_ptr = *iter;
      break;
    }
  }

  // Create Handler
  handler = new ReverseProxyHandler();

  // Create Reply
  std::unique_ptr<Reply> reply = handler->HandleRequest(request);

  // Test Handler's reply
  EXPECT_EQ(reply->http_ver(), "HTTP/1.1");
  EXPECT_EQ(reply->status_code(), 500);
  EXPECT_EQ(reply->body(), "500 Internal Server Error\r\n");
  EXPECT_EQ((reply->headers())["Content-Type"], "text/plain");
}
