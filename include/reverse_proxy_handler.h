#pragma once

#include "handler.h"
#include "reply.h"
#include "request.h"

/**
    Handler for serving static files.
*/
class ReverseProxyHandler : public Handler {
 public:
  /**
      Factory method for creating a ReverseProxyHandler pointer.

      @param config the config block for a ReverseProxyHandler.
      @param root_path the root_path for a ReverseProxyHandler.
      @return a ReverseProxyHandler pointer.
  */
  static ReverseProxyHandler* create(const NginxConfig& config,
                               const std::string& root_path);
  /**
      Handles a request.

      @param request the config block for a ReverseProxyHandler.
      @return a Reply unique pointer.
  */
  std::unique_ptr<Reply> HandleRequest(const Request& request);

 private:
   // uri prefix for request handler, equivalent to location in handler blocks in config
  std::string uri_prefix;
  // path prefix for requested handler, equivalent to root in handler blocks in config
  std::string path_prefix;
  // path of a server root
  std::string root_path;
  // extra server host name
  std::string host_name;
  // extra server port number
  std::string port_name;
  /**
      Given a url received from request, get a file path to a static file
      requested by client

      @param url the url received from request
      @return path to the requested static file
  */
  std::string GetPath(std::string url);
  /**
      Given an http response, get the status portion of the message

      @param resp the full http response from the host
      @return status code of response
  */
  int GetStatus(const std::string& resp);
  /**
      Given an http response, get the body portion of the message

      @param resp the full http response from the host
      @param start represents the start of the body (which is the last position this function
             wants to consider)
      @return body of the response
  */
  std::map<std::string, std::string> GetHeaders(const std::string& resp, const size_t& start);
  /**
      Given an http response, get the body portion of the message

      @param resp the full http response from the host
      @param start represents the start of the body (which is near the first position this
             function wants to consider)
      @return body of the response
  */
  std::string GetBody(const std::string& resp, const size_t& start);
  /**
      Given a config block and the root path of the server, initialize data members

      @param config config block of requested handler
      @param root_path root path of the server
      @return true if succeeed, false otherwise
  */
  bool Init(const NginxConfig& config, const std::string& root_path);
  /**
      Given a config block and the root path of the server, initialize data members

      @param ec error code from resolving the query
      @return iterator for boost::asio::connect()
  */
  boost::asio::ip::tcp::resolver::iterator ResolveHost(boost::system::error_code& ec);
  /**
      Given an error code, this prints out a message detailing why certain boost asio calls failed

      @param rep the reply that is sent out
      @param ec error code from some boost asio function
      @return iterator for boost::asio::connect()
  */
  std::unique_ptr<Reply> ReturnError(std::unique_ptr<Reply> rep, const boost::system::error_code& ec);
};
