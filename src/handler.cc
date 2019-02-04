#include "handler.h"
#include "response.h"
#include "request.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

std::map<std::string, Handler* (*)(void)>* request_handler_builders = nullptr;

Handler* Handler::GetHandlerByType(const char* type) {
  const auto type_and_builder = request_handler_builders->find(type);
  if (type_and_builder == request_handler_builders->end()) {
    return nullptr;
  }
  return (*type_and_builder->second)();
}



Handler::bool EchoHandler::Init(const std::string& uri_prefix, const NginxConfig& config) {
    return true;
}


//Echo handler
Handler::bool EchoHandler::HandleRequest(const Request& request, Response* response) {
    std::cout << "\nEchoHandler::HandleRequest" << std::endl;

    response->SetStatus(200);
    response->AddHeader("Content-Type", "text/plain");
    response->SetBody(request.original_request());
    return true;
}



bool StaticHandler::Init(const std::string& uri_prefix, const NginxConfig& config){
  return false;
}

bool StaticHandler::HandleRequest(const Request& request, const NginxConfig& config){
  return false;
}



Handler::bool NotFoundHandler::Init(const std::string& uri_prefix, const NginxConfig& config) {
    return true;
}

//Not Found handler 
Handler::bool NotFoundHandler::HandleRequest(const Request& request, Response* response){
    std::cout << "\nNotFoundHandler::HandleRequest" << std::endl;

    std::string body = "404 NOT FOUND";
    response->SetStatus(404);
    response->AddHeader("Content-Type", "text/plain");
    response->SetBody(body);
    return true;
}
