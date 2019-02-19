#include "config_parser.h"
#include "server.h"
#include "echo_handler.h"
#include "static_handler.h"
#include "not_found_handler.h"

Server::Server(boost::asio::io_service& io_service, unsigned short port, const NginxConfig config)
  : io_service_(io_service),
    acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
    config_(config){
  start_accept();
}


void Server::start_accept() {
  Session* new_session = new Session(io_service_, config_, &request_map_);
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&Server::handle_accept, this, new_session,
            boost::asio::placeholders::error));
}

void Server::handle_accept(Session* new_session,
    const boost::system::error_code& error) {
    if (!error) {
        new_session->start();
        new_session = new Session(io_service_, config_, &request_map_);
        acceptor_.async_accept(new_session->socket(),
        boost::bind(&Server::handle_accept, this, new_session,
        boost::asio::placeholders::error));

    } else {
        delete new_session;
    }

    start_accept();
}
