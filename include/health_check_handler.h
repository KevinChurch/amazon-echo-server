#pragma once

#include "handler.h"
#include "reply.h"
#include "request.h"

/**
    Handler for handling 404 errors.
*/
class HealthCheckHandler : public Handler {
 public:
  /**
      Factory method for creating a HealthCheckHandler pointer.

      @param config the config block for a HealthCheckHandler.
      @param root_path the root_path for a HealthCheckHandler.
      @return a HealthCheckHandler pointer.
  */
  static HealthCheckHandler* create(const NginxConfig& config,
                                 const std::string& root_path);
  /**
      Handles a request.

      @param request the config block for a HealthCheckHandler.
      @return a Reply unique pointer.
  */
  std::unique_ptr<Reply> HandleRequest(const Request& request);
};
