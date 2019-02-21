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
  std::string uri_prefix;
  std::string path_prefix;
  std::string root_path;
  std::string GetPath(std::string url);
  bool IsRegularFile(std::string path);
  std::string GetContentType(std::string file_name);
  std::string GetContent(std::ifstream& file);
  bool Init(const NginxConfig& config, const std::string& root_path);
};
