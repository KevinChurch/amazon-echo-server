#include "handler.h"
#include "response.h"
#include "request.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

bool EchoHandler::Init(const std::string& uri_prefix, const NginxConfig& config) {
    return true;
}

bool EchoHandler::HandleRequest(const Request& request, Response* response) {
    std::cout << "\nEchoHandler::HandleRequest" << std::endl;

    response->SetStatus(200);
    response->SetHeader("Content-Type", "text/plain");
    response->SetBody(request.original_request());
    return true;
}


bool StaticHandler::Init(const std::string& uri_prefix, const NginxConfig& config){
  return true;
}

bool StaticHandler::HandleRequest(const Request& request, Response* response){
  return false;
}



bool NotFoundHandler::Init(const std::string& uri_prefix, const NginxConfig& config) {
    return true;
}

bool NotFoundHandler::HandleRequest(const Request& request, Response* response){
    std::cout << "\nNotFoundHandler::HandleRequest" << std::endl;

    std::string body = "404 NOT FOUND\r\n";
    response->SetStatus(404);
    response->SetHeader("Content-Type", "text/plain");
    response->SetBody(body);
    return true;
}
