#include "config_parser.h"
#include "server.h"
#include "echo_handler.h"
#include "static_handler.h"
#include "not_found_handler.h"

Server::Server(boost::asio::io_service& io_service, unsigned short port, const NginxConfig config)
  : io_service_(io_service),
    acceptor_(io_service, tcp::endpoint(tcp::v4(), port)){
  
  //This is where the the server will set up its configuration
  //TODO: Use appropriate Nginx Parser functions
  std::string echo_uri = config.Find("server.path1.location");
  std::string static_uri1 = config.Find("server.path2.location");
  std::string static_uri2 = config.Find("server.path3.location");
  std::string not_found_uri = config.Find("server.path4.location");
  std::string echo_root;
  std::string static_root1 = config.Find("server.path2.root");
  std::string static_root2 = config.Find("server.path3.root");
  std::string not_found_root;
  boost::shared_ptr<EchoHandler> echo_ptr(EchoHandler::create(config, echo_root));
  boost::shared_ptr<StaticHandler> static_ptr1(StaticHandler::create(config, static_root1));
  boost::shared_ptr<StaticHandler> static_ptr2(StaticHandler::create(config, static_root2));
  boost::shared_ptr<NotFoundHandler> not_found_ptr(NotFoundHandler::create(config, not_found_uri));

  handler_map_[echo_uri] = echo_ptr;
  handler_map_[static_uri1] = static_ptr1;
  handler_map_[static_uri2] = static_ptr2;
  handler_map_[not_found_uri] = not_found_ptr;
  start_accept();
}
      

void Server::start_accept() {
  Session* new_session = new Session(io_service_, handler_map_);
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&Server::handle_accept, this, new_session,
            boost::asio::placeholders::error));
}

void Server::handle_accept(Session* new_session,
    const boost::system::error_code& error) {
    if (!error) {
        new_session->start();
        new_session = new Session(io_service_, handler_map_);
        acceptor_.async_accept(new_session->socket(),
        boost::bind(&Server::handle_accept, this, new_session,
        boost::asio::placeholders::error));

    } else {
        delete new_session;
    }

    start_accept();
}
