#pragma once

#include "handler.h"
#include "reply.h"
#include "request.h"

/**
    Handler for handling 404 errors.
*/
class SleepHandler : public Handler {
 public:
  /**
      Factory method for creating a SleepHandler pointer.

      @param config the config block for a SleepHandler.
      @param root_path the root_path for a SleepHandler.
      @return a SleepHandler pointer.
  */
  static SleepHandler* create(const NginxConfig& config,
                                 const std::string& root_path);
  /**
      Handles a request.

      @param request the config block for a SleepHandler.
      @return a Reply unique pointer.
  */
  std::unique_ptr<Reply> HandleRequest(const Request& request);
};
