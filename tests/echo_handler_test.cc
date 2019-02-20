#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include "gtest/gtest.h"
#include "echo_handler.h"

class EchoHandlerTest : public ::testing::Test {
  protected:
    
    NginxConfig config;
    std::string request_string = "GET /echo HTTP/1.1\r\n\r\n";
    std::unique_ptr<Request> request_ = Request::ParseRequest(request_string);
    
    EchoHandler* handler_ = EchoHandler::create(config, "static");
    std::unique_ptr<Reply> reply_ptr = handler_->HandleRequest(*request_);
    
    
};

TEST_F(EchoHandlerTest, SimpleEchoRequest) {
  
  
  EXPECT_EQ("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nGET /echo HTTP/1.1\r\n\r\n", reply_ptr->ToString());

  
}

TEST_F(EchoHandlerTest, NotEchoRequest) {
  
  
  EXPECT_NE("HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\n\r\nGET /echo HTTP/1.1\r\n\r\n", reply_ptr->ToString());

  
}



