#include "gtest/gtest.h"
#include "handler.h"
#include "not_found_handler.h"
#include "request.h"
#include "reply.h"
#include "config_parser.h"

class NotFoundHandlerTest : public ::testing::Test {
protected:
  NotFoundHandler* handler;
  Request request;
  NginxConfig config;
  NginxConfig* handler_config_ptr;
  NginxConfigParser config_parser;

};

TEST_F(NotFoundHandlerTest, HandleRequest) {
  
  //Parse example config
  config_parser.Parse("new_example_config", &config);

  //Create Request
  request.ParseRequest("GET / HTTP/1.1\r\n\r\n");

  //Find Not Found Handler in config
  std::vector<NginxConfig*> handlers = config.FindBlocks("handler");
  for (std::vector<NginxConfig*>::iterator iter = handlers.begin(); iter != handlers.end(); iter++){
    if ((*iter)->Find("name") == "not_found"){
      	handler_config_ptr = *iter;
	break;
    }
  }

  //Create Handler
  handler = new NotFoundHandler();
  
  //Create Reply
  std::unique_ptr<Reply> reply = handler->HandleRequest(request);
  
  //Test Handler's reply
  EXPECT_EQ(reply->http_ver(),"HTTP/1.1");
  EXPECT_EQ(reply->status_code(),404);
  EXPECT_EQ(reply->body(),"404 NOT FOUND\r\n");
  EXPECT_EQ((reply->headers())["Content-Type"], "text/plain");  
}
