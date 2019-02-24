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
      Given a url received from request, get a file path to a static file
      requested by client

      @param url the url received from request
      @return path to the requested static file
  */
  bool Init(const NginxConfig& config, const std::string& root_path);
};
