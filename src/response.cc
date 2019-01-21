#include "response.h"

Response::Response(void){
  m_http_ver = "HTTP/1.1";
}

void Response::SetStatus(int status){
  m_status_code = status;
}

void Response::SetHeader(std::string key, std::string value){
  m_headers[key] = value;
}

void Response::SetBody(std::string body){
  m_body = body;
}

int Response::status_code(void) const{
  return m_status_code;
}

std::string Response::body(void) const{
  return m_body;
}

std::map<std::string, std::string> Response::headers(void) const{
  return m_headers;
}

std::string Response::http_ver(void) const{
  return m_http_ver;
}

std::string Response::ToString(void) const{
  std::string s = "";

  s += this->http_ver();
  s += " ";

  s += std::to_string(this->status_code());
  s += " ";

  if(this->status_code() == 200){
    s += "OK";
  }

  s += "\r\n";

  //Add Headers
  for (auto it = (this->m_headers).begin();
       it != (this->m_headers).end(); ++it){
    s += it->first;
    s += ": ";
    s += it->second;
    s += "\r\n";
  }

  s += "\r\n";

  //Add body
  s += this->body();
  return s;
}
