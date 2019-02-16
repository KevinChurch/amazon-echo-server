#include "echo_handler.h"
#include "response.h"
#include "request.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>

EchoHandler* EchoHandler::create(const NginxConfig& config, const std::string& root_path) {
  return new EchoHandler;
}

bool EchoHandler::HandleRequest(const Request& request, Response* response) {
    std::cout << "\nEchoHandler::HandleRequest" << std::endl;

    response->SetStatus(200);
    response->SetHeader("Content-Type", "text/plain");
    response->SetBody(request.original_request());
    return true;
}