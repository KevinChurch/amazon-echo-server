#pragma once

#include "handler.h"
#include "reply.h"
#include "request.h"

// status handler to display information on status of the web server
class StatusHandler : public Handler {
 public:
  static StatusHandler* create(const NginxConfig& config,
                               const std::string& root_path);
  std::unique_ptr<Reply> HandleRequest(const Request& request);
  void setRequestInfo(std::map<std::string, std::map<int, int>>* request_info);
  void setHandlers(std::vector<NginxConfig*> handlers);

 private:
  std::string requestInfoToString();
  std::string handlersToString();
  std::map<std::string, std::map<int, int>>* request_info_map;
  std::vector<NginxConfig*> handlers;
};
