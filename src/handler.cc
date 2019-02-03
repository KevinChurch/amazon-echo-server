#include "handler.h"
#include "response.h"
#include "request.h"

Handler* Handler::GetHandlerByType(char* type){

  return nullptr;
}

bool EchoHandler::Init(const std::string& uri_prefix, const NginxConfig& config){
  return false;
}

bool EchoHandler::HandleRequest(const Request& request, const NginxConfig& config){
  return false;
}

bool StaticHandler::Init(const std::string& uri_prefix, const NginxConfig& config){
  return false;
}

bool StaticHandler::HandleRequest(const Request& request, const NginxConfig& config){
  return false;
}

bool NotFoundHandler::Init(const std::string& uri_prefix, const NginxConfig& config){
  return false;
}

bool NotFoundHandler::HandleRequest(const Request& request, const NginxConfig& config){
  return false;
}
