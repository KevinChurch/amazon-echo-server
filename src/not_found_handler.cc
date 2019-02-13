#include "not_found_handler.h"

bool NotFoundHandler::Init(const NginxConfig& config) {
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
