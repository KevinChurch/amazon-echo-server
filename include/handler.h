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
  // virtual static Handler *make_handler(const NginxConfig& config) = 0;
  Handler* GetHandlerByType(const char* type);
  virtual bool Init(const NginxConfig& config) = 0;
  virtual bool HandleRequest(const Request& request, Response* response) = 0;
};