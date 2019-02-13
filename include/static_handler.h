#pragma once

#include "handler.h"

class StaticHandler: public Handler {
public:
  bool Init(const NginxConfig& config);
  bool Init(const NginxConfig& config, std::string uri_prefix);
  bool HandleRequest(const Request& request, Response* response);

private:
  std::string uri_prefix;
  std::string path_prefix;
  std::string GetPath(std::string url);
  bool IsRegularFile(std::string path);
  std::string GetContentType(std::string file_name);
  std::string GetContent(std::ifstream& file);
};