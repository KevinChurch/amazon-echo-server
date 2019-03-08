#pragma once
#include <memory>
#include <string>
#include <vector>
#include <map>

class Request {
 public:
  static std::unique_ptr<Request> ParseRequest(
      const std::string& original_request);

  std::string original_request() const;
  std::string method() const;
  std::string uri() const;
  void set_uri(std::string url);
  std::string version() const;
  void set_params(std::string body);
  std::map<std::string, std::string> params() const;

  // we are using headers and body in response.cc file, I will delete this
  // comment later using Headers = std::vector<std::pair<std::string,
  // std::string>>; Headers headers() const; std::string body() const;

 private:
  std::string m_original_request;
  std::string m_method;
  std::string m_uri;
  std::string m_version;
  std::string m_body;
  std::map<std::string, std::string> m_params;
  // std::string m_body;
  // Headers m_headers;
};
