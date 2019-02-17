#pragma once

#include "handler.h"

// simple handler to echo back raw reply
class EchoHandler : public Handler {
public:
  static EchoHandler* create(const NginxConfig& config, const std::string& root_path);
  std::unique_ptr<Reply> HandleRequest(const Request& request);

private:
  std::string to_send;
};