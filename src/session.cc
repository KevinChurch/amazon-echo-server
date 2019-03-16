#include "session.h"
#include "handler_manager.h"
#include "logging.h"
#include "reply.h"
#include "request.h"

#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <memory>
#define quote(x) #x

Session::Session(boost::asio::io_service& io_service, const NginxConfig config,
                 std::map<std::string, std::map<int, int>>* request_map)
    : socket_(io_service), config(config), request_map(request_map) {}

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
    socket_.async_read_some(
        boost::asio::buffer(data_, max_length),
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

int Session::handle_request() {
  BOOST_LOG_SEV(my_logger::get(), INFO) << "Handling request";

  auto request = Request::ParseRequest(get_message_request());
  std::string s = socket_.remote_endpoint().address().to_string();
  std::string original_url = request->uri();

  NginxConfig* handler_config;
  unsigned int longest_match_size = 0;
  std::string handler_name = "";
  std::unique_ptr<Reply> reply_ptr = nullptr;
  
  //If the request is valid
  if(request->valid()){
    // Find longest matching URI
    std::vector<NginxConfig*> handlers = config.FindBlocks("handler");
    for (std::vector<NginxConfig*>::iterator iter = handlers.begin();
	 iter != handlers.end(); iter++) {
      std::string prefix = (*iter)->Find("location");
      if (original_url.find(prefix) == 0) {
	if (prefix.length() > longest_match_size) {
	  longest_match_size = prefix.length();
	  handler_name = (*iter)->Find("name");
	  handler_config = *iter;
	}
      }
    }

    BOOST_LOG_SEV(my_logger::get(), INFO) << "Creating a handler";
    manager.setRequestMap(request_map);
    manager.setHandlers(handlers);
    std::unique_ptr<Handler> handler_ptr(
					 manager.createByName(handler_name, *handler_config, config.Find("root")));

    reply_ptr = handler_ptr->HandleRequest(*request);

    // If failure, use NotFound Handler
    if (!reply_ptr) {
      std::unique_ptr<Handler> error_ptr(
					 manager.createByName("not_found", this->config, config.Find("root")));
      reply_ptr = error_ptr->HandleRequest(*request);
    }
  }
  //Else Request is invalid, use NotFound Handler
  else{
      std::unique_ptr<Handler> error_ptr(
					 manager.createByName("not_found", this->config, config.Find("root")));
      reply_ptr = error_ptr->HandleRequest(*request);
  }
  // putting the request to the request_map
  if (request_map->find(request->uri()) != request_map->end()) {
    if ((*request_map)[request->uri()].find(reply_ptr->status_code()) !=
        (*request_map)[request->uri()].end()) {
      (*request_map)[request->uri()][reply_ptr->status_code()] += 1;
    } else {
      (*request_map)[request->uri()][reply_ptr->status_code()] = 1;
    }
  } else {
    (*request_map)[request->uri()][reply_ptr->status_code()] = 1;
  }

  write_string(reply_ptr->ToString());

  std::string original_request_str = request->original_request();
  while (!original_request_str.empty() &&
             original_request_str[original_request_str.size() - 1] == '\n' ||
         original_request_str[original_request_str.size() - 1] == '\r') {
    original_request_str.erase(original_request_str.size() - 1);
  }

  BOOST_LOG_SEV(my_logger::get(), INFO) << s << " - - " << original_request_str
                                        << ' ' << reply_ptr->status_code();

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

std::string Session::get_message_request() {
  std::string msg{buffers_begin(buffer.data()), buffers_end(buffer.data())};

  return msg;
}
