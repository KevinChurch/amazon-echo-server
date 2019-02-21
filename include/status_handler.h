#pragma once

#include "handler.h"
#include "reply.h"
#include "request.h"

/**
    Handler for displaying web server's status information.
*/
class StatusHandler : public Handler {
 public:
  /**
      Factory method for creating a StaticHandler pointer.

      @param config the config block for a StaticHandler.
      @param root_path the root_path for a StaticHandler.
      @return a StaticHandler pointer.
  */
  static StatusHandler* create(const NginxConfig& config,
                               const std::string& root_path);
  /**
      Handles a request.

      @param request the config block for a StaticHandler.
      @return a Reply unique pointer.
  */
  std::unique_ptr<Reply> HandleRequest(const Request& request);
  void setRequestInfo(std::map<std::string, std::map<int, int>>* request_info);
  void setHandlers(std::vector<NginxConfig*> handlers);

 private:
  std::string requestInfoToString();
  std::string handlersToString();
  std::map<std::string, std::map<int, int>>* request_info_map;
  std::vector<NginxConfig*> handlers;
};
