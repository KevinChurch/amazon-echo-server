#pragma once

#include <boost/asio.hpp>
#include "response.h"
#include "request.h"
#include "config_parser.h"

class Handler{
 public:
  Handler* GetHandlerByType(const char* type);
  virtual bool Init(const NginxConfig& config) = 0;
  virtual bool HandleRequest(const Request& request, Response* response) = 0;
};

// simple handler to echo back raw response
class EchoHandler : public Handler {
public:
  EchoHandler() {}
  bool Init(const NginxConfig& config);
  bool HandleRequest(const Request& request, Response* response);

private:
    std::string to_send;
};

class StaticHandler: public Handler {
public:
  StaticHandler() {}
  bool Init(const NginxConfig& config);
  bool Init(const NginxConfig& config, std::string uri_prefix);
  bool HandleRequest(const Request& request, Response* response);

private:
  std::string uri_prefix;
  std::string path_prefix;
  std::string GetPath(std::string url);
  bool RegularFile(std::string path);
  std::string GetContentType(std::string file_name);
  std::string GetContent(std::ifstream& file);
};

class NotFoundHandler : public Handler {
public:
  NotFoundHandler() {}
  bool Init(const NginxConfig& config);
  bool HandleRequest(const Request& request, Response* response);
};
