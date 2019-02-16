#pragma once

#include "handler.h"

// simple handler to echo back raw response
class EchoHandler : public Handler {
public:
  static EchoHandler* create(const NginxConfig& config, const std::string& root_path);
  bool HandleRequest(const Request& request, Response* response);

private:
  std::string to_send;
};