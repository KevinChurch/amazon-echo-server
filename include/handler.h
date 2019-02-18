#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include "reply.h"
#include "request.h"
#include "config_parser.h"
#include "logging.h"

class Handler {
public:
  static Handler* create(const NginxConfig& config, const std::string& root_path);
  virtual std::unique_ptr<Reply> HandleRequest(const Request& request) = 0;
};