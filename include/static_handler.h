#pragma once

#include "handler.h"
#include "reply.h"
#include "request.h"

/**
    Handler for serving static files.
*/
class StaticHandler : public Handler {
 public:
  /**
      Factory method for creating a StaticHandler pointer.

      @param config the config block for a StaticHandler.
      @param root_path the root_path for a StaticHandler.
      @return a StaticHandler pointer.
  */
  static StaticHandler* create(const NginxConfig& config,
                               const std::string& root_path);
  /**
      Handles a request.

      @param request the config block for a StaticHandler.
      @return a Reply unique pointer.
  */
  std::unique_ptr<Reply> HandleRequest(const Request& request);

 private:
   // uri prefix for request handler, equivalent to root in handler blocks in config
  std::string uri_prefix;
  // path prefix for requested handler, equivalent to root in handler blocks in config
  std::string path_prefix;
  // path of a server root
  std::string root_path;
  /**
      Given a url received from request, get a file path to a static file
      requested by client

      @param url the url received from request
      @return path to the requested static file
  */
  std::string GetPath(std::string url);
  /**
      Given a file paht, determine if the file is a regular file

      @param path path to the requested static file
      @return true if regular file, false if not a regular file
  */
  bool IsRegularFile(std::string path);
  /**
      Given a file_name, get the file extension to determing the content type

      @param file_name file name
      @return content type
  */
  std::string GetContentType(std::string file_name);
  /**
      Given a file, get the content of the file

      @param file file to get content from
      @return content of the file
  */
  std::string GetContent(std::ifstream& file);
  /**
      Given a config block and the root path of the server, initialize data members

      @param config config block of requested handler
      @param root_path root path of the server
      @return true if succeeed, false otherwise
  */
  bool Init(const NginxConfig& config, const std::string& root_path);
};
