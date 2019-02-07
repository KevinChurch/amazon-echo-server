#include "handler.h"
#include "response.h"
#include "request.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>


bool EchoHandler::Init(const NginxConfig& config) {
  return true;
}

bool EchoHandler::HandleRequest(const Request& request, Response* response) {
    std::cout << "\nEchoHandler::HandleRequest" << std::endl;

    response->SetStatus(200);
    response->SetHeader("Content-Type", "text/plain");
    response->SetBody(request.original_request());
    return true;
}


bool StaticHandler::Init(const NginxConfig& config){
  return this->Init(config, "/static");
}


bool StaticHandler::Init(const NginxConfig& config, std::string uri_prefix){
  this->uri_prefix = uri_prefix;
  if (config.Find("server.path2.location") == uri_prefix)
    this->path_prefix = config.Find("server.path2.root");
  else if (config.Find("server.path3.location") == uri_prefix)
    this->path_prefix = config.Find("server.path3.root");
  else{//failed to find matching root
    this->path_prefix = "";
    return false;
  }
  return true;
}

bool StaticHandler::HandleRequest(const Request& request, Response* response){
  std::cout << "\nStaticHandler::HandleRequest" << std::endl;

  std::string file_path = GetPath(request.uri());
  std::ifstream file;

  if(!IsRegularFile(file_path)){
    std::cerr << "Error: Static File requested, but file is not regular" << std::endl;
    return false;
  }

  file.open(file_path);
  if(!file.is_open()){
    std::cerr << "Error: File could not be opened" << std::endl;
    return false;
  }

  std::string file_content = GetContent(file);
  response->SetStatus(200);
  response->SetHeader("Content-Type", GetContentType(file_path));
  response->SetBody(file_content);

  return true;
}

std::string StaticHandler::GetPath(std::string url){
  //We assume the url has the prefix declared in the StaticHandler
  int prefix_len = this->uri_prefix.length();
  std::string path = this->path_prefix + url.substr(prefix_len);

  return path;
}

bool StaticHandler::IsRegularFile(std::string path){
  //Check if requested path is a regular file
  struct stat path_stat;

  stat(path.c_str(), &path_stat);
  if(!S_ISREG(path_stat.st_mode)){
    return false;
  }

  return true;
}
std::string StaticHandler::GetContentType(std::string file_name){
  std::size_t ext_pos = file_name.rfind(".");

  if (ext_pos == std::string::npos){
    return "text/plain";
  }

  std::string ext = file_name.substr(ext_pos);
  std::string type;


  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

  if(!ext.compare(".bmp"))
    type = "image/bmp";
  else if(!ext.compare(".css"))
    type = "text/css";
  else if(!ext.compare(".csv"))
    type = "text/csv";
  else if(!ext.compare(".doc"))
    type = "application/msword";
  else if(!ext.compare(".docx"))
    type = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
  else if(!ext.compare(".gif"))
    type = "image/gif";
  else if(!ext.compare(".htm") || !ext.compare(".html"))
    type = "text/html";
  else if(!ext.compare(".jpg") || !ext.compare(".jpeg"))
    type = "image/jpeg";
  else if(!ext.compare(".mp3"))
    type = "audio/mpeg";
  else if(!ext.compare(".mpeg"))
    type = "video/mpeg";
  else if(!ext.compare(".png"))
    type = "image/png";
  else if(!ext.compare(".pdf"))
    type = "application/pdf";
  else if(!ext.compare(".ppt"))
    type = "application/vnd.ms-powerpoint";
  else if(!ext.compare(".pptx"))
    type = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
  else if(!ext.compare(".rar"))
    type = "application/x-rar-compressed";
  else if(!ext.compare(".rtf"))
    type = "application/rtf";
  else if(!ext.compare(".sh"))
    type = "application/x-sh";
  else if(!ext.compare(".tar"))
    type = "application/x-tar";
  else if(!ext.compare(".txt"))
    type = "text/plain";
  else if(!ext.compare(".wav"))
    type = "audio/wav";
  else if(!ext.compare(".xhtml"))
    type = "application/xhtml+xml";
  else if(!ext.compare(".xls"))
    type = "application/vnd.ms-excel";
  else if(!ext.compare(".xlsx"))
    type = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
  else if(!ext.compare(".xml"))
    type = "text/xml";
  else if(!ext.compare(".zip"))
    type = "application/zip";
  else
    type = "text/plain";

  return type;

}

std::string StaticHandler::GetContent(std::ifstream& file){
  std::string content;

  //Resereve just enough space for contents in string
  file.seekg(0, std::ios::end);
  content.reserve(file.tellg());
  file.seekg(0, std::ios::beg);

  content.assign((std::istreambuf_iterator<char>(file)),
	     std::istreambuf_iterator<char>());

  file.close();

  return content;
}



bool NotFoundHandler::Init(const NginxConfig& config) {
    return true;
}

bool NotFoundHandler::HandleRequest(const Request& request, Response* response){
    std::cout << "\nNotFoundHandler::HandleRequest" << std::endl;

    std::string body = "404 NOT FOUND\r\n";
    response->SetStatus(404);
    response->SetHeader("Content-Type", "text/plain");
    response->SetBody(body);
    return true;
}
