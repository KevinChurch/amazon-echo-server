#include "not_found_handler.h"

NotFoundHandler* NotFoundHandler::create(const NginxConfig& config, const std::string& root_path) {
    return new NotFoundHandler;
}

bool NotFoundHandler::HandleRequest(const Request& request, Reply* reply){
    std::cout << "\nNotFoundHandler::HandleRequest" << std::endl;

    std::string body = "404 NOT FOUND\r\n";
    reply->SetStatus(404);
    reply->SetHeader("Content-Type", "text/plain");
    reply->SetBody(body);
    return true;
}
