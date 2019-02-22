#include "server.h"
#include "config_parser.h"
#include "echo_handler.h"
#include "not_found_handler.h"
#include "static_handler.h"
#include <boost/thread/thread.hpp>

Server::Server(boost::asio::io_service& io_service, unsigned short port,
               const NginxConfig config, std::size_t thread_pool_size)
  : io_service_(io_service),
    acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
    config_(config),
    thread_pool_size_(thread_pool_size){
  start_accept();
}

void Server::run()
{
  // Create a pool of threads to run all of the io_services.
  std::vector<boost::shared_ptr<boost::thread> > threads;
  for (std::size_t i = 0; i < thread_pool_size_; ++i)
    {
      boost::shared_ptr<boost::thread> thread(new boost::thread(
	   boost::bind(&boost::asio::io_service::run, &io_service_)));
      threads.push_back(thread);
    }

  // Wait for all threads in the pool to exit.
  for (std::size_t i = 0; i < threads.size(); ++i)
    threads[i]->join();
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
    acceptor_.async_accept(
        new_session->socket(),
        boost::bind(&Server::handle_accept, this, new_session,
                    boost::asio::placeholders::error));

  } else {
    delete new_session;
  }

  start_accept();
}
