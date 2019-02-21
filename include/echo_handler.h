#pragma once

#include "handler.h"
#include "reply.h"
#include "request.h"

/**
    Handler for echoing requests.
*/
class EchoHandler : public Handler {
 public:
  /**
      Factory method for creating an EchoHandler pointer.

      @param config the config block for an EchoHandler.
      @param root_path the root_path for an EchoHandler.
      @return a EchoHandler pointer.
  */
  static EchoHandler* create(const NginxConfig& config,
                             const std::string& root_path);
  /**
      Handles a request.

      @param request the config block for an EchoHandler.
      @return a Reply unique pointer.
  */
  std::unique_ptr<Reply> HandleRequest(const Request& request);
};
