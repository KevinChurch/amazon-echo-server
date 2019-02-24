#include "handler_manager.h"
#include "config_parser.h"
#include "echo_handler.h"
#include "handler.h"
#include "not_found_handler.h"
#include "reply.h"
#include "request.h"
#include "static_handler.h"
#include "status_handler.h"
#include "reverse_proxy_handler.h"

// Registration mechanism. Your server must have a delegate class
// which takes care of registering creation factories for handlers.
// (Name chosen here is “HandlerManager” but you do not need to name
// your class that exact name.) This registration class is just a factory
// that is meant to interface between handlers and the server code.

// This “do it” method returns a pointer to the request handler. The
// caller takes ownership of the object.
// (Handler class may be called something different.)
// name : the name given in the config file, e.g. for “handler foo { … }”
//   name is "foo"
// config : the config block given for this handler in the config file.
//   This is the contents of the block following the declaration of the
//   named handler in the file.
// path : the global absolute path root for the server.

std::unique_ptr<Handler> HandlerManager::createByName(
    const std::string& name, const NginxConfig& config,
    const std::string& root_path) {
  std::unique_ptr<Handler> handler_ptr(nullptr);

  if (name == "echo") {
    handler_ptr.reset(EchoHandler::create(config, root_path));
  } else if (name == "static") {
    handler_ptr.reset(StaticHandler::create(config, root_path));
  } else if (name == "not_found") {
    handler_ptr.reset(NotFoundHandler::create(config, root_path));
  } else if (name == "reverse_proxy") {
    handler_ptr.reset(ReverseProxyHandler::create(config, root_path));
  } else if (name == "status") {
    // handler_ptr.reset(StatusHandler::create(config, root_path));
    StatusHandler* st_handler = StatusHandler::create(config, root_path);
    st_handler->setRequestInfo(this->handler_request_map);
    st_handler->setHandlers(this->handlers);
    handler_ptr.reset(st_handler);
    // delete st_handler;
  }

  return handler_ptr;
}
// Implementation should accomplish something like the following simple
// case. It’s possible to have more elaborate mechanisms, but the mechanism
// of registering a handler should be equivalent to editing a single file
// and performing a very formulaic addition like this:
//   case “echo”: return EchoHandler::create(config, path);
//   case “static”: return StaticHandler::create(config, path);
//   case “404”: return 404Handler::create(config, path)

void HandlerManager::setRequestMap(
    std::map<std::string, std::map<int, int>>* request_map) {
  this->handler_request_map = request_map;
}

void HandlerManager::setHandlers(std::vector<NginxConfig*> handlers) {
  this->handlers = handlers;
}
