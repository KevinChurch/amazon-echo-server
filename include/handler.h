#pragma once

#include <boost/asio.hpp>
#include "response.h"
#include "request.h"
#include "config_parser.h"
#include <map>

class Handler{
 public:
  Handler* GetHandlerByType(const char* type);
  virtual bool Init(const std::string& uri_prefix, const NginxConfig& config) = 0;
  virtual bool HandleRequest(const Request& request, const NginxConfig& config) = 0;
}


extern std::map<std::string, Handler* (*)(void)>* request_handler_builders;
template<typename T>
class RequestHandlerRegisterer {
 public:
  RequestHandlerRegisterer(const std::string& type) {
    if (request_handler_builders == nullptr) {
      request_handler_builders = new std::map<std::string, Handler* (*)(void)>;
    }
    (*request_handler_builders)[type] = RequestHandlerRegisterer::Create;
  }
  static Handler* Create() {
    return new T;
  }
};
#define REGISTER_REQUEST_HANDLER(ClassName) \
  static RequestHandlerRegisterer<ClassName> ClassName##__registerer(#ClassName)



// simple handler to echo back raw response
class EchoHandler : public Handler {
public:
    EchoHandler() {}

    bool Init(const std::string& uri_prefix, const NginxConfig& config);

    bool HandleRequest(const Request& request, Response* response);
private:
    std::string to_send;
};

REGISTER_REQUEST_HANDLER(EchoHandler);



class StaticHandler: public Handler {
 public:
  StaticHandler() {}
}


REGISTER_REQUEST_HANDLER(StaticHandler);

class NotFoundHandler : public Handler {
public:
    NotFoundHandler() {}

    bool Init(const std::string& uri_prefix, const NginxConfig& config);

    bool HandleRequest(const Request& request, Response* response);
};


REGISTER_REQUEST_HANDLER(NotFoundHandler);