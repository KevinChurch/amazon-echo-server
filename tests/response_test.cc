#include "gtest/gtest.h"
#include "response.h"

class ResponseTest : public ::testing::Test {
protected:
  Response resp;
  bool success;
};

TEST_F(ResponseTest, HTTPVersion) {
  resp = Response();

  EXPECT_EQ(resp.http_ver(), "HTTP/1.1");
}

TEST_F(ResponseTest, StatusCode) {
  resp = Response();
  resp.SetStatus(200);

  EXPECT_EQ(resp.status_code(), 200);
}

TEST_F(ResponseTest, Header) {
  resp = Response();
  resp.SetHeader("Content-Type", "text/plain");
  resp.SetHeader("My-Header", "some/value");


  EXPECT_EQ((resp.headers())["Content-Type"], "text/plain");
  EXPECT_EQ((resp.headers())["My-Header"], "some/value");
}

TEST_F(ResponseTest, Body) {
  resp = Response();
  resp.SetBody("This is my\nbody");

  EXPECT_EQ(resp.body(), "This is my\nbody");
}

TEST_F(ResponseTest, ToString) {
  resp = Response();
  resp.SetStatus(200);
  resp.SetHeader("Content-Type", "text/plain");
  resp.SetBody("This is my\nbody");
  
  std::string expected = "HTTP/1.1 200 OK\r\n";
  expected += "Content-Type: text/plain\r\n";
  expected += "\r\n";
  expected += "This is my\nbody";

  EXPECT_EQ(resp.ToString(), expected);
}

