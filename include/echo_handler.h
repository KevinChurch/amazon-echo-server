#pragma once

#include "handler.h"

// simple handler to echo back raw response
class EchoHandler : public Handler {
public:
  bool Init(const NginxConfig& config);
  bool HandleRequest(const Request& request, Response* response);

private:
    std::string to_send;
};