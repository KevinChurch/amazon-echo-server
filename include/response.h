#ifndef RESPONSE_DEFINED
#define RESPONSE_DEFINED

#include <string>
#include <map>


class Response{
 public:
  Response(void);
  void SetStatus(int);
  void SetHeader(std::string key, std::string value);
  void SetBody(std::string);
  std::string http_ver(void) const;
  int status_code(void) const;
  std::string body(void) const;
  std::map<std::string, std::string> headers(void) const;
  std::string ToString(void) const;

 private:
  std::string m_http_ver;
  int m_status_code;
  std::map<std::string, std::string> m_headers;
  std::string m_body;

};

#endif

