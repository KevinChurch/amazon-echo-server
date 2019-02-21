#pragma once

#include <sys/stat.h>
#include <sys/types.h>
#include <boost/asio.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "config_parser.h"
#include "logging.h"
#include "reply.h"
#include "request.h"

class Handler {
 public:
  static Handler* create(const NginxConfig& config,
                         const std::string& root_path);
  virtual std::unique_ptr<Reply> HandleRequest(const Request& request) = 0;
};