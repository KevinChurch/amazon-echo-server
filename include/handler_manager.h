#pragma once

#include "config_parser.h"
#include "handler.h"
#include "reply.h"
#include "request.h"

// Registration mechanism. Your server must have a delegate class
// which takes care of registering creation factories for handlers.
// (Name chosen here is “HandlerManager” but you do not need to name
// your class that exact name.) This registration class is just a factory
// that is meant to interface between handlers and the server code.
class HandlerManager {
  // This “do it” method returns a pointer to the request handler. The
  // caller takes ownership of the object.
  // (Handler class may be called something different.)
  // name : the name given in the config file, e.g. for “handler foo { … }”
  //   name is "foo"
  // config : the config block given for this handler in the config file.
  //   This is the contents of the block following the declaration of the
  //   named handler in the file.
  // path : the global absolute path root for the server.
 public:
  std::unique_ptr<Handler> createByName(const std::string& name,
                                        const NginxConfig& config,
                                        const std::string& root_path);
  void setRequestMap(std::map<std::string, std::map<int, int>>* request_map);
  void setHandlers(std::vector<NginxConfig*> handlers);

  // Implementation should accomplish something like the following simple
  // case. It’s possible to have more elaborate mechanisms, but the mechanism
  // of registering a handler should be equivalent to editing a single file
  // and performing a very formulaic addition like this:
  //   case “echo”: return EchoHandler::create(config, path);
  //   case “static”: return StaticHandler::create(config, path);
  //   case “404”: return 404Handler::create(config, path)

 private:
  std::map<std::string, std::map<int, int>>* handler_request_map;
  std::vector<NginxConfig*> handlers;
};
