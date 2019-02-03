#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include "handler.h" 
#include "session.h"
#include <map>

using boost::asio::ip::tcp;

class Server {
public:
  Server(boost::asio::io_service& io_service, short port,
  	std::map<std::string, std::pair<std::string, std::shared_ptr<RequestHandler>>> handler_map);

private:
  void start_accept();

  void handle_accept(Session* new_session,
      const boost::system::error_code& error);

  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
  std::map <std::string, std::pair<std::string, std::shared_ptr<RequestHandler>>> handler_map_;
};

#endif