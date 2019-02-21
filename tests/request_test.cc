#include "request.h"
#include <iostream>
#include <sstream>
#include <string>
#include "gtest/gtest.h"
using namespace std;

class RequestTest : public ::testing::Test {
 protected:
  string s = "GET / HTTP/1.1\r\n\r\n";
  string t = "GET /static/index.html HTTP/1.1\r\n\r\n";
  string u = "GET /static1/amazon.jpg HTTP/1.1\r\n\r\n";
  string v = "GET |static/index.html HTTP/1.1\r\n\r\n";
  string r = "GO | HTTP/1.0\r\n\r\n";
  string w = " / HTTP/1.1\r\n\r\n";
  string x = "GET  / HTTP/1.1\r\n\r\n";
  unique_ptr<Request> request = Request::ParseRequest(s);
  unique_ptr<Request> static_request = Request::ParseRequest(t);
  unique_ptr<Request> static1_request = Request::ParseRequest(u);
  unique_ptr<Request> invalid_request = Request::ParseRequest(r);
  unique_ptr<Request> invalid_static_request = Request::ParseRequest(v);
  unique_ptr<Request> request_space = Request::ParseRequest(w);
  unique_ptr<Request> request_doublespace = Request::ParseRequest(x);
  bool succes;
};

TEST_F(RequestTest, CheckOriginalRequestStartWithSapce) {
  EXPECT_EQ(request_space->original_request(), "");
}

TEST_F(RequestTest, CheckOriginalRequestStartWithDoubleSapces) {
  EXPECT_EQ(request_doublespace->original_request(), "");
}
TEST_F(RequestTest, CheckOriginalRequest) {
  EXPECT_EQ(request->original_request(), "GET / HTTP/1.1\r\n\r\n");
}

TEST_F(RequestTest, CheckInvalidOriginalRequest) {
  EXPECT_NE(invalid_request->original_request(), "GET / HTTP/1.1\r\n\r\n");
}

TEST_F(RequestTest, CheckMethod) { EXPECT_EQ(request->method(), "GET"); }

TEST_F(RequestTest, CheckInvaildMethod) {
  EXPECT_NE(invalid_request->method(), "GET");
}

TEST_F(RequestTest, CheckUri) { EXPECT_EQ(request->uri(), "/"); }

TEST_F(RequestTest, CheckInvalidUri) { EXPECT_NE(invalid_request->uri(), "/"); }

TEST_F(RequestTest, CheckStaticFileUri) {
  EXPECT_EQ(static_request->uri(), "/static/index.html");
  EXPECT_EQ(static1_request->uri(), "/static1/amazon.jpg");
}

TEST_F(RequestTest, CheckInvalidStaticFileUri) {
  EXPECT_NE(invalid_static_request->uri(), "/static/index.html");
}

TEST_F(RequestTest, CheckVersion) { EXPECT_EQ(request->version(), "HTTP/1.1"); }

TEST_F(RequestTest, CheckInvalidVersion) {
  EXPECT_NE(invalid_request->version(), "HTTP/1.1");
}
