#include "static_handler.h"

StaticHandler* StaticHandler::create(const NginxConfig& config,
                                     const std::string& root_path) {
  StaticHandler* handler = new StaticHandler;
  handler->Init(config, root_path);
  return handler;
}

bool StaticHandler::Init(const NginxConfig& config,
                         const std::string& root_path) {
  this->root_path = root_path;
  this->path_prefix = config.Find("root");
  this->uri_prefix = config.Find("location");

  return true;
}

std::unique_ptr<Reply> StaticHandler::HandleRequest(const Request& request) {
  std::cout << "\nStaticHandler::HandleRequest" << std::endl;
  BOOST_LOG_SEV(my_logger::get(), INFO) << "\nStaticHandler::HandleRequest";

  std::unique_ptr<Reply> reply_ptr(new Reply());
  std::string file_path = GetPath(request.uri());
  std::ifstream file;

  if (!IsRegularFile(file_path)) {
    std::cerr << "Error: Static File requested, but file is not regular"
              << std::endl;
    BOOST_LOG_SEV(my_logger::get(), ERROR)
        << "Static File requested, but file is not regular";
    return nullptr;
  }

  file.open(file_path);
  if (!file.is_open()) {
    std::cerr << "Error: File could not be opened" << std::endl;
    BOOST_LOG_SEV(my_logger::get(), ERROR) << "File could not be opened";
    return nullptr;
  }

  std::string file_content = GetContent(file);
  reply_ptr->SetStatus(200);
  reply_ptr->SetHeader("Content-Type", GetContentType(file_path));
  reply_ptr->SetBody(file_content);
  return reply_ptr;
}

std::string StaticHandler::GetPath(std::string url) {
  // We assume the url has the prefix declared in the StaticHandler
  int prefix_len = this->uri_prefix.length();
  std::string path =
      this->root_path + this->path_prefix + url.substr(prefix_len);

  return path;
}

bool StaticHandler::IsRegularFile(std::string path) {
  // Check if requested path is a regular file
  struct stat path_stat;

  stat(path.c_str(), &path_stat);
  if (!S_ISREG(path_stat.st_mode)) {
    return false;
  }

  return true;
}
std::string StaticHandler::GetContentType(std::string file_name) {
  std::size_t ext_pos = file_name.rfind(".");

  if (ext_pos == std::string::npos) {
    return "text/plain";
  }

  std::string ext = file_name.substr(ext_pos);
  std::string type;

  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

  if (!ext.compare(".bmp"))
    type = "image/bmp";
  else if (!ext.compare(".css"))
    type = "text/css";
  else if (!ext.compare(".csv"))
    type = "text/csv";
  else if (!ext.compare(".doc"))
    type = "application/msword";
  else if (!ext.compare(".docx"))
    type =
        "application/"
        "vnd.openxmlformats-officedocument.wordprocessingml.document";
  else if (!ext.compare(".gif"))
    type = "image/gif";
  else if (!ext.compare(".htm") || !ext.compare(".html"))
    type = "text/html";
  else if (!ext.compare(".jpg") || !ext.compare(".jpeg"))
    type = "image/jpeg";
  else if (!ext.compare(".mp3"))
    type = "audio/mpeg";
  else if (!ext.compare(".mpeg"))
    type = "video/mpeg";
  else if (!ext.compare(".png"))
    type = "image/png";
  else if (!ext.compare(".pdf"))
    type = "application/pdf";
  else if (!ext.compare(".ppt"))
    type = "application/vnd.ms-powerpoint";
  else if (!ext.compare(".pptx"))
    type =
        "application/"
        "vnd.openxmlformats-officedocument.presentationml.presentation";
  else if (!ext.compare(".rar"))
    type = "application/x-rar-compressed";
  else if (!ext.compare(".rtf"))
    type = "application/rtf";
  else if (!ext.compare(".sh"))
    type = "application/x-sh";
  else if (!ext.compare(".tar"))
    type = "application/x-tar";
  else if (!ext.compare(".txt"))
    type = "text/plain";
  else if (!ext.compare(".wav"))
    type = "audio/wav";
  else if (!ext.compare(".xhtml"))
    type = "application/xhtml+xml";
  else if (!ext.compare(".xls"))
    type = "application/vnd.ms-excel";
  else if (!ext.compare(".xlsx"))
    type = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
  else if (!ext.compare(".xml"))
    type = "text/xml";
  else if (!ext.compare(".zip"))
    type = "application/zip";
  else
    type = "text/plain";

  return type;
}

std::string StaticHandler::GetContent(std::ifstream& file) {
  std::string content;

  // Resereve just enough space for contents in string
  file.seekg(0, std::ios::end);
  content.reserve(file.tellg());
  file.seekg(0, std::ios::beg);

  content.assign((std::istreambuf_iterator<char>(file)),
                 std::istreambuf_iterator<char>());

  file.close();

  return content;
}
