#pragma once

#include "handler.h"

class NotFoundHandler : public Handler {
public:
  static NotFoundHandler* create(const NginxConfig& config, const std::string& root_path);
  std::unique_ptr<Reply> HandleRequest(const Request& request);
};