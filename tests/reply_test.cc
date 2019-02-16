#include "gtest/gtest.h"
#include "reply.h"

class ReplyTest : public ::testing::Test {
protected:
  Reply resp;
  bool success;
};

TEST_F(ReplyTest, HTTPVersion) {
  resp = Reply();

  EXPECT_EQ(resp.http_ver(), "HTTP/1.1");
}

TEST_F(ReplyTest, StatusCode) {
  resp = Reply();
  resp.SetStatus(200);

  EXPECT_EQ(resp.status_code(), 200);
}

TEST_F(ReplyTest, Header) {
  resp = Reply();
  resp.SetHeader("Content-Type", "text/plain");
  resp.SetHeader("My-Header", "some/value");


  EXPECT_EQ((resp.headers())["Content-Type"], "text/plain");
  EXPECT_EQ((resp.headers())["My-Header"], "some/value");
}

TEST_F(ReplyTest, Body) {
  resp = Reply();
  resp.SetBody("This is my\nbody");

  EXPECT_EQ(resp.body(), "This is my\nbody");
}

TEST_F(ReplyTest, ToString) {
  resp = Reply();
  resp.SetStatus(200);
  resp.SetHeader("Content-Type", "text/plain");
  resp.SetBody("This is my\nbody");
  
  std::string expected = "HTTP/1.1 200 OK\r\n";
  expected += "Content-Type: text/plain\r\n";
  expected += "\r\n";
  expected += "This is my\nbody";

  EXPECT_EQ(resp.ToString(), expected);
}

