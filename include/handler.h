#pragma once

#include <boost/asio.hpp>
#include "response.h"
#include "request.h"
#include "config_parser.h"

class Handler{
 public:
  Handler* GetHandlerByType(const char* type);
  virtual bool Init(const std::string& uri_prefix, const NginxConfig& config) = 0;
  virtual bool HandleRequest(const Request& request, Response* response) = 0;
};

// simple handler to echo back raw response
class EchoHandler : public Handler {
public:
  EchoHandler() {}
  bool Init(const std::string& uri_prefix, const NginxConfig& config);
  bool HandleRequest(const Request& request, Response* response);

private:
    std::string to_send;
};

class StaticHandler: public Handler {
public:
  StaticHandler() {}
  bool Init(const std::string& uri_prefix, const NginxConfig& config);
  bool HandleRequest(const Request& request, Response* response);

private:
  std::string uri_prefix;
  std::string path;
  std::string get_path_from_url(std::string url);
  bool file_exists(const char *path);
  std::string get_content_type(std::string file);
  std::string read_file(std::string file);
};

class NotFoundHandler : public Handler {
public:
  NotFoundHandler() {}
  bool Init(const std::string& uri_prefix, const NginxConfig& config);
  bool HandleRequest(const Request& request, Response* response);
};
