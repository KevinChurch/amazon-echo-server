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

/**
    Abstract class for all Handlers.
*/
class Handler {
 public:
  /**
      Factory method for creating a Handler pointer.

      @param config the config block for a Handler.
      @param root_path the root_path for a Handler.
      @return a Handler pointer.
  */
  static Handler* create(const NginxConfig& config,
                         const std::string& root_path);
  /**
      Handles a request.

      @param request the config block for a Handler.
      @return a Reply unique pointer.
  */
  virtual std::unique_ptr<Reply> HandleRequest(const Request& request) = 0;
};