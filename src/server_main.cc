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

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
  try {
    if (argc != 2) {
      std::cerr << "Usage: ./server <path to config file>\n";
      return 1;
    }

    NginxConfigParser config_parser;
    NginxConfig config;
    config_parser.Parse(argv[1], &config);

    boost::asio::io_service io_service;

    unsigned short port = stoi(config.Find("server.listen"));
    Server s(io_service, port, config);

    io_service.run();
  }catch (ConfigParserException& e) {
    std::cerr << "Exception: " << e.what() << "\n";
    exit(1);
  }catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
