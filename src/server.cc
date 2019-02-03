#include "server.h"
#include <iostream>
#include <boost/shared_ptr.hpp>

Server::Server(boost::asio::io_service& io_service, short port, 
    std::map <std::string, std::pair<std::string, std::shared_ptr<RequestHandler>>> handler_map)
    : io_service_(io_service),
    acceptor_(io_service, tcp::endpoint(tcp::v4(), port),handler_map_(handler_map)) {
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
        boost::bind(&server::handle_accept, this, new_session,
        boost::asio::placeholders::error));

    } else {
        delete new_session;
    }

    start_accept();
}