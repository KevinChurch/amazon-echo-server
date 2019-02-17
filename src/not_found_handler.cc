#include "not_found_handler.h"
#include "reply.h"
#include "request.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>

NotFoundHandler* NotFoundHandler::create(const NginxConfig& config, const std::string& root_path) {
  return new NotFoundHandler;
}

std::unique_ptr<Reply> NotFoundHandler::HandleRequest(const Request& request) {
  std::cout << "\nNotFoundHandler::HandleRequest" << std::endl;
  std::unique_ptr<Reply> reply_ptr(new Reply());

  std::string body = "404 NOT FOUND\r\n";
  reply_ptr->SetStatus(404);
  reply_ptr->SetHeader("Content-Type", "text/plain");
  reply_ptr->SetBody(body);
  return reply_ptr;
}