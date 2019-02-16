#include "echo_handler.h"
#include "reply.h"
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

bool EchoHandler::HandleRequest(const Request& request, Reply* reply) {
    std::cout << "\nEchoHandler::HandleRequest" << std::endl;

    reply->SetStatus(200);
    reply->SetHeader("Content-Type", "text/plain");
    reply->SetBody(request.original_request());
    return true;
}