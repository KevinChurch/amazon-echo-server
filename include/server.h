#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <map>
#include "config_parser.h"
#include "session.h"

#define DEFAULT_THREAD_NUM 4

using boost::asio::ip::tcp;

class Server {
 public:
  Server(boost::asio::io_service& io_service,
	 unsigned short port,
         const NginxConfig config,
	 std::size_t thread_pool_size);
  void run(void);

private:
  void start_accept();

  void handle_accept(Session* new_session,
                     const boost::system::error_code& error);

  std::size_t thread_pool_size_;
  NginxConfig config_;
  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
  std::map<std::string, std::map<int, int>> request_map_;
};

#endif
