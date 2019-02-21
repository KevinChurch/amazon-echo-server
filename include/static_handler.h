#pragma once

#include "handler.h"
#include "reply.h"
#include "request.h"

class StaticHandler : public Handler {
 public:
  static StaticHandler* create(const NginxConfig& config,
                               const std::string& root_path);
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
