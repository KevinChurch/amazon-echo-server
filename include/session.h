#ifndef SESSION_H
#define SESSION_H

#include <cstdlib>
#include <iostream>
#include <map>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Session {
public:
  Session(boost::asio::io_service& io_service, 
    std::map <std::string, std::pair<std::string, std::shared_ptr<RequestHandler>>> handler_map);

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
    std::string get_longest_prefix(const std::string original_url);
    boost::asio::streambuf buffer;

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
};

#endif