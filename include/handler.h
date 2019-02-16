#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include "response.h"
#include "request.h"
#include "config_parser.h"

class Handler {
public:
  static Handler* create(const NginxConfig& config, const std::string& root_path);
  virtual bool HandleRequest(const Request& request, Response* response) = 0;
};