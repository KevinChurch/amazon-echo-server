#pragma once

#include "handler.h"

class NotFoundHandler : public Handler {
public:
  bool Init(const NginxConfig& config);
  bool HandleRequest(const Request& request, Response* response);
};