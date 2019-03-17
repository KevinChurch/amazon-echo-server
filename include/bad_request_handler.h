#pragma once

#include "handler.h"
#include "reply.h"
#include "request.h"

/**
    Handler for handling 404 errors.
*/
class BadRequestHandler : public Handler {
 public:
  /**
      Factory method for creating a BadRequestHandler pointer.

      @param config the config block for a BadRequestHandler.
      @param root_path the root_path for a BadRequestHandler.
      @return a BadRequestHandler pointer.
  */
  static BadRequestHandler* create(const NginxConfig& config,
                                 const std::string& root_path);
  /**
      Handles a request.

      @param request the config block for a BadRequestHandler.
      @return a Reply unique pointer.
  */
  std::unique_ptr<Reply> HandleRequest(const Request& request);
};
