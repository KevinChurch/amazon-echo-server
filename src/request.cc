#include "request.h"
#include <iostream>
#include <vector>
#include "logging.h"

// implementation for ParseRequest
std::unique_ptr<Request> Request::ParseRequest(
    const std::string& original_request) {
  std::unique_ptr<Request> req(new Request);
  req->m_original_request = original_request;
  std::string method;
  std::string version;
  std::string url;
  std::string body;
  int count = 0;

  for (unsigned int i = 0; i < original_request.size(); i++) {
    if (original_request[i] == '\r') break;

    if (original_request[i] == ' ') {
      if (i == 0) {
        req->m_original_request = "";
        std::cerr << "400 BAD REQUEST!" << std::endl;
        BOOST_LOG_SEV(my_logger::get(), ERROR) << "400 BAD REQUEST!";
        method = "";
        url = "";
        version = "";
        break;
      }
      if (original_request[i - 1] == ' ') {
        req->m_original_request = "";
        std::cerr << "400 BAD REQUEST!" << std::endl;
        BOOST_LOG_SEV(my_logger::get(), ERROR) << "400 BAD REQUEST!";
        method = "";
        url = "";
        version = "";
        break;
      }
      count++;
      continue;
    }

    switch (count) {
      case 0:
        method += original_request[i];
        break;
      case 1:
        url += original_request[i];
        break;
      case 2:
        version += original_request[i];
        break;
    }
  }

  if (method != "GET" && method != "POST") {
    std::cerr << "Invalid method request!" << std::endl;
    BOOST_LOG_SEV(my_logger::get(), ERROR) << "Invalid method request!";
    req->m_original_request = "";
  }

  if (version != "HTTP/1.1") {
    std::cerr << "HTTP Version not support!" << std::endl;
    BOOST_LOG_SEV(my_logger::get(), ERROR) << "HTTP Version not support!";
    req->m_original_request = "";
  }

  if (url.size() < 1) req->m_original_request = "";

  // parse body
  size_t headers_end = original_request.find("\r\n\r\n");
  body = original_request.substr(headers_end+4);

  req->m_method = method;
  req->m_uri = url;
  req->m_version = version;
  req->m_body = body;

  // parse parameters from body
  req->set_params(body);

  return req;
}

void Request::set_params(std::string body) {
  std::map<std::string, std::string> params_map;
  size_t pos = body.find("=");
  while (pos != std::string::npos) {
    std::string key = body.substr(0, pos);
    body = body.substr(pos+1);

    std::string value;
    pos = body.find("&");
    if (pos != std::string::npos)
      value = body.substr(0, pos);
    else
      value = body;

    params_map[key] = value;

    pos = body.find("=");
  }

  m_params = params_map;
}

std::string Request::original_request() const { return m_original_request; }

std::string Request::method() const { return m_method; }

std::string Request::uri() const { return m_uri; }

void Request::set_uri(std::string url) { m_uri = url; }

std::string Request::version() const { return m_version; }

std::map<std::string, std::string> Request::params() const { return m_params; }

/*	we are using headers and body in response.cc file, I will delete this
comment later using Headers = std::vector<std::pair<std::string, std::string>>;
Headers Request::headers() const {
        return m_headers;
}

std::string Request::body() const {
        return m_body;
}
*/
