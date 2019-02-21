#pragma once

#include "handler.h"
#include "reply.h"
#include "request.h"

/**
    Handler for handling 404 errors.
*/
class NotFoundHandler : public Handler {
 public:
  /**
      Factory method for creating a StaticHandler pointer.

      @param config the config block for a StaticHandler.
      @param root_path the root_path for a StaticHandler.
      @return a StaticHandler pointer.
  */
  static NotFoundHandler* create(const NginxConfig& config,
                                 const std::string& root_path);
  /**
      Handles a request.

      @param request the config block for a StaticHandler.
      @return a Reply unique pointer.
  */
  std::unique_ptr<Reply> HandleRequest(const Request& request);
};
