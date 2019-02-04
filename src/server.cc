#include "config_parser.h"
#include "server.h"

Server::Server(boost::asio::io_service& io_service, unsigned short port, const NginxConfig config)
  : io_service_(io_service),
    acceptor_(io_service, tcp::endpoint(tcp::v4(), port)){
  
  //This is where the the server will set up its configuration
  //TODO: Use appropriate Nginx Parser functions
  boost::shared_ptr<EchoHandler> echo_ptr(new EchoHandler);
  boost::shared_ptr<StaticHandler> static_ptr(new StaticHandler);
  echo_ptr->Init(config);
  static_ptr->Init(config);
  handler_map_["/echo"] = echo_ptr;
  handler_map_["/static"] = static_ptr;
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
