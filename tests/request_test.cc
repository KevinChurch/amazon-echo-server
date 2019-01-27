#include "gtest/gtest.h"
#include "request.h"
#include <iostream>
#include <string>
#include <sstream>
using namespace std;



class RequestTest:public::testing::Test{
	protected:
		string s = "GET / HTTP/1.1\r\n\r\n";
		string r = "GO | HTTP/1.0\r\n\r\n";
		unique_ptr<Request> request = Request::ParseRequest(s);
		unique_ptr<Request> invalidrequest = Request::ParseRequest(r);
		bool succes;

};


TEST_F(RequestTest, CheckOriginalRequest){
	EXPECT_EQ(request->original_request(),"GET / HTTP/1.1\r\n\r\n");
}

TEST_F(RequestTest, CheckInvalidOriginalRequest){
	EXPECT_NE(invalidrequest->original_request(),"GET / HTTP/1.1\r\n\r\n");
}

TEST_F(RequestTest, CheckMethod){
	EXPECT_EQ(request->method(),"GET");
}

TEST_F(RequestTest, CheckInvaildMethod){
	EXPECT_NE(invalidrequest->method(),"GET");
}

TEST_F(RequestTest, CheckUri){
	EXPECT_EQ(request->uri(),"/");
}

TEST_F(RequestTest, CheckInvalidUri){
	EXPECT_NE(invalidrequest->uri(),"/");
}

TEST_F(RequestTest, CheckVersion){
	EXPECT_EQ(request->version(),"HTTP/1.1");
}

TEST_F(RequestTest, CheckInvalidVersion){
	EXPECT_NE(invalidrequest->version(),"HTTP/1.1");
}




