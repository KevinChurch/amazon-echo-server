#include "session.h"
#include "request.h"
#include "response.h"

#include <termios.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <memory>

Session::Session(boost::asio::io_service& io_service)
    : socket_(io_service) { }

void Session::handle_read(const boost::system::error_code& error,
    size_t bytes_transferred) {
    if (!error) {
        boost::asio::async_write(socket_,
            boost::asio::buffer(data_, bytes_transferred),
            boost::bind(&Session::handle_write, this,
            boost::asio::placeholders::error));
    } else {
        delete this;
    }
}

void Session::handle_write(const boost::system::error_code& error) {
    if (!error) {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            boost::bind(&Session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    } else {
        delete this;
    }
}

void Session::start() {
//    socket_.async_read_some(boost::asio::buffer(data_, max_length),
//        boost::bind(&Session::handle_read, this,
//            boost::asio::placeholders::error,
//            boost::asio::placeholders::bytes_transferred));
boost::asio::async_read_until(socket_, buffer, "\r\n\r\n",
      boost::bind(&Session::handle_request, this));
}

int Session::handle_request(){

  auto request = Request::ParseRequest(get_message_request());
  Response response = Response();
  response.SetStatus(200);
  response.SetHeader("Content-Type", "text/plain");
  response.SetBody(request->original_request());
  write_string(response.ToString());
  return 0;
}



void Session::write_string(std::string send) {

  boost::asio::streambuf out_streambuf;
  std::ostream out(&out_streambuf);
  out << send;

  boost::asio::write(socket_, out_streambuf);

 
  boost::system::error_code ec;
  socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  socket_.close();
}




std::string Session::get_message_request()
{
  std::string msg{
    buffers_begin(buffer.data()),
    buffers_end(buffer.data())
  };

  return msg;
}
