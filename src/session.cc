#include "session.h"
#include "request.h"
#include "reply.h"
#include "logging.h"
#include "not_found_handler.h"

#include <termios.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <memory>

Session::Session(boost::asio::io_service& io_service,
  std::map <std::string, boost::shared_ptr<Handler>> handler_map)
    : socket_(io_service), handler_map(handler_map) { }

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
boost::asio::async_read_until(socket_, buffer, "\r\n\r\n",
      boost::bind(&Session::handle_request, this));
}

int Session::handle_request(){
  Reply reply = Reply();
  auto request = Request::ParseRequest(get_message_request());
  std::string s = socket_.remote_endpoint().address().to_string();

  std::string longest_prefix = get_longest_prefix(request->uri());
  boost::shared_ptr<Handler> handler_ptr = handler_map[longest_prefix];
  if (!handler_ptr){
    handler_ptr.reset(new NotFoundHandler);
  }

  bool success = handler_ptr->HandleRequest(*request, &reply);

  //If failure, use NotFound Handler
  if(!success){
    handler_ptr.reset(new NotFoundHandler);
    handler_ptr->HandleRequest(*request, &reply);
  }

  write_string(reply.ToString());

  std::string original_request_str = request->original_request();
	while(!original_request_str.empty()
	      && original_request_str[original_request_str.size() - 1] == '\n'
	      || original_request_str[original_request_str.size() - 1] == '\r') {
		original_request_str.erase(original_request_str.size() - 1);
	}

  INFO << s << " - - " << original_request_str << ' ' << reply.status_code();

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


std::string Session::get_longest_prefix(const std::string original_url)
{
  unsigned int longest_match_size = 0;
  std::string longest_match = "";

  for (auto const& iter : handler_map){
    if (original_url.find(iter.first) == 0){
      if (iter.first.length() > longest_match_size){
        longest_match = iter.first;
        longest_match_size = iter.first.length();
      }
    }
  }
  return longest_match;
}
