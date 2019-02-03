#pragma once

#include <boost/asio.hpp>
#include "response.h"
#include "request.h"
#include "config_parser.h"

class Handler{
 public:
  Handler* GetHandlerByType(const char* type);
  virtual bool Init(const std::string& uri_prefix, const NginxConfig& config) = 0;
  virtual bool HandleRequest(const Request& request, const NginxConfig& config) = 0;
}



class EchoHandler: public Handler {
 public:
  EchoHandler() {}
}

class StaticHandler: public Handler {
 public:
  StaticHandler() {}
}

class NotFoundHandler: public Handler{ 
  public:
   NotFoundHandler() {}
}