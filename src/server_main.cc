//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "config_parser.h"
#include "server.h"
#include "config_parser_exception.h"
#include "logging.h"

using boost::asio::ip::tcp;

void handler( const boost::system::error_code& error , int signal_number )
{
  if (signal_number == 2) {
    BOOST_LOG_SEV(my_logger::get(), INFO) << "Server has stoppped by user";
  }

  exit(1);
}

int main(int argc, char* argv[])
{
  try {
    if (argc != 2) {
      std::cerr << "Usage: ./server <path to config file>\n";
      BOOST_LOG_SEV(my_logger::get(), ERROR) << "Wrong number of arguments were provided by the user";
      return 1;
    }

    BOOST_LOG_SEV(my_logger::get(), INFO) << "Server started running...";

    NginxConfigParser config_parser;
    NginxConfig config;
    config_parser.Parse(argv[1], &config);

    boost::asio::io_service io_service;

    unsigned short port = stoi(config.Find("port"));

    BOOST_LOG_SEV(my_logger::get(), INFO) << "Successfully parsed a config file";
    BOOST_LOG_SEV(my_logger::get(), INFO) << "Running the server on port " << port;

    // Construct a signal set registered for process termination.
    boost::asio::signal_set signals(io_service, SIGINT);
    // Start an asynchronous wait for one of the signals to occur.
    signals.async_wait(handler);

    Server s(io_service, port, config);

    io_service.run();
  }catch (ConfigParserException& e) {
    std::cerr << "Exception: " << e.what() << "\n";
    BOOST_LOG_SEV(my_logger::get(), ERROR) << "Config parser exception: " << e.what();
    exit(1);
  }catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
    BOOST_LOG_SEV(my_logger::get(), ERROR) << "Exception: " << e.what();
    exit(1);
  }

  return 0;
}
