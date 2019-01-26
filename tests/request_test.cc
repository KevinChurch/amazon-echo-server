#include "gtest/gtest.h"
#include "request.h"
#include <iostream>
#include <string>
#include <sstream>
using namespace std;



class RequestTest:public::testing::Test{
	protected:
		string s = "GET / HTTP/1.1\r\n\r\n";
		bool succes;

};


TEST_F(RequestTest, CheckOriginal_request){
	auto request = Request::ParseRequest(s);
	EXPECT_EQ(request->original_request(),"GET / HTTP/1.1\r\n\r\n");
}

TEST_F(RequestTest, CheckMethod){
	auto request = Request::ParseRequest(s);
	EXPECT_EQ(request->method(),"GET");
}

TEST_F(RequestTest, ChechkUri){
	auto request = Request::ParseRequest(s);
	EXPECT_EQ(request->uri(),"/");
}

TEST_F(RequestTest, CheckVersion){
	auto request = Request::ParseRequest(s);
	EXPECT_EQ(request->version(),"HTTP/1.1");
}




