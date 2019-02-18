#pragma once

#include "handler.h"
#include "handler_manager.h"
#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Session {
public:
  Session(boost::asio::io_service& io_service, const NginxConfig config);

  tcp::socket& socket() {
    return socket_;
  }

  void start();
  int handle_request();
  void write_string(std::string send);

private:
  void handle_read(const boost::system::error_code& error,
		   size_t bytes_transferred);

  void handle_write(const boost::system::error_code& error);

  std::string get_message_request();
  boost::asio::streambuf buffer;

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  HandlerManager manager;
  const NginxConfig config;
};
