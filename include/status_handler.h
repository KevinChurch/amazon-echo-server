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
      Factory method for creating a StatusHandler pointer.

      @param config the config block for a StatusHandler.
      @param root_path the root_path for a StatusHandler.
      @return a StatusHandler pointer.
  */
  static StatusHandler* create(const NginxConfig& config,
                               const std::string& root_path);
  /**
      Handles a request.

      @param request the config block for a StatusHandler.
      @return a Reply unique pointer.
  */
  std::unique_ptr<Reply> HandleRequest(const Request& request);

  /**
      Given a pointer to information of all requests received from Handler Manager,
      set it as request_info_map so Status Handler can retrieve and display
      the information when requested

      @param request_info a pointer to a map of <URL(string), <STATUS_CODE(int), #_OF_REQUESTS_RECEIVED(int)>>
  */
  void setRequestInfo(std::map<std::string, std::map<int, int>> * request_info);
  /**
      Given information of all handlers currently exist in the server from
      Handler Manager and set it as a data member handlers.

      @param handlers a vector of config blocks containing information of handlers
  */
  void setHandlers(std::vector<NginxConfig*> handlers);

private:
  /**
      Convert request_info_map into a displayable string

      @return a string representation of information of requests received
  */
  std::string requestInfoToString();
  /**
      Convert handlers into a displayable string

      @return a string representation of information of handlers
  */
  std::string handlersToString();
  std::map<std::string, std::map<int, int>>* request_info_map;
  std::vector<NginxConfig*> handlers;
};
