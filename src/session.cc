#include "handler_manager.h"
#include "session.h"
#include "request.h"
#include "reply.h"
#include "logging.h"

#include <termios.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <memory>
#define quote(x) #x

Session::Session(boost::asio::io_service& io_service, const NginxConfig config)
    : socket_(io_service), config(config) { }

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
  auto request = Request::ParseRequest(get_message_request());
  std::string s = socket_.remote_endpoint().address().to_string();
  std::string original_url = request->uri();
  
  NginxConfig *handler_config;
  unsigned int longest_match_size = 0;
  std::string handler_name = "";
  

  //Find longest matching URI
  std::vector<NginxConfig*> handlers = config.FindBlocks("handler");
  for (std::vector<NginxConfig*>::iterator iter = handlers.begin(); iter != handlers.end(); iter++){
    std::string prefix = (*iter)->Find("location");
    if (original_url.find(prefix) == 0){
      if (prefix.length() > longest_match_size){
	longest_match_size = prefix.length();
	handler_name = (*iter)->Find("name");
	handler_config = *iter;
      }
    }
  }

  std::unique_ptr<Handler> handler_ptr(manager.createByName(handler_name,
							    *handler_config,
							    config.Find("root")));

  std::unique_ptr<Reply> reply_ptr = handler_ptr->HandleRequest(*request);

  //If failure, use NotFound Handler
  if(!reply_ptr){
  std::unique_ptr<Handler> error_ptr(manager.createByName("not_found",
							  this->config,
							  config.Find("root")));
    reply_ptr = error_ptr->HandleRequest(*request);
  }

  write_string(reply_ptr->ToString());

  std::string original_request_str = request->original_request();
  while(!original_request_str.empty()
	&& original_request_str[original_request_str.size() - 1] == '\n'
	|| original_request_str[original_request_str.size() - 1] == '\r') {
    original_request_str.erase(original_request_str.size() - 1);
  }

  BOOST_LOG_SEV(my_logger::get(), INFO) << s << " - - " << original_request_str << ' ' << reply_ptr->status_code();

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

