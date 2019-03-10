#include "meme_handler.h"
#include "database.h"
#include <vector>
#include "html_builder.h"

MemeHandler* MemeHandler::create(const NginxConfig& config,
                                 const std::string& root_path) {
  BOOST_LOG_SEV(my_logger::get(), INFO) << "Creating a MemeHandler";
  MemeHandler* handler = new MemeHandler;
  handler->Init(config, root_path);
  return handler;

  return new MemeHandler;
}

bool MemeHandler::Init(const NginxConfig& config,
                       const std::string& root_path) {

  this->database = InitDatabase();
  // root path for the server
  this->root_path = root_path;
  // root path for the directory for meme files
  this->path_prefix = config.Find("root");
  // uri_prefix for current handler
  this->uri_prefix = config.Find("location");

  return true;
}

std::unique_ptr<Database> MemeHandler::InitDatabase() {
  std::unique_ptr<Database> db(new Database());

  return db;
}

std::unique_ptr<Reply> MemeHandler::HandleRequest(const Request& request) {
  std::cout << "\nMemeHandler::HandleRequest" << std::endl;
  BOOST_LOG_SEV(my_logger::get(), INFO) << "\nMemeHandler::HandleRequest";
  BOOST_LOG_SEV(my_logger::get(), INFO) << "Received URI: " << request.uri();
  std::unique_ptr<Reply> reply_ptr(new Reply());
  std::string request_uri = request.uri();

  std::string action_str = request_uri.substr(this->uri_prefix.length());
  std::string params_str;
  std::string file_path;
  std::string body;
  std::map<std::string, std::string> params;

  size_t pos = action_str.find("/");
  if (pos == std::string::npos) {
    BOOST_LOG_SEV(my_logger::get(), ERROR) << "Not a valid path for Meme Handler";
    return nullptr;
  }
  else {
    action_str = action_str.substr(pos+1);
  }

  pos = action_str.find("?");
  if(pos != std::string::npos) {
    params_str = action_str.substr(pos+1);
    action_str = action_str.substr(0,pos);
  }

  pos = action_str.find("/");
  if(pos != std::string::npos) {
    action_str = action_str.substr(0,pos);
  }

  if (action_str == "new") {
    BOOST_LOG_SEV(my_logger::get(), INFO) << "MemeHandler::HandleRequest - handling /meme/new";
    // get content of /static/memes/assets/new.html
    file_path = "./static/memes/new.html";
    body = ReadFromFile(file_path);
    // set the body
    reply_ptr->SetBody(body);
    reply_ptr->SetHeader("Content-Type", "text/html");
  }
  else if (action_str == "create") {
    BOOST_LOG_SEV(my_logger::get(), INFO) << "MemeHandler::HandleRequest - handling /meme/create";
    // check if the method is POST
    if (request.method() != "POST") {
      BOOST_LOG_SEV(my_logger::get(), ERROR) << "Not a valid method for request to /meme/create";
      return nullptr;
    }
    // get params from request
    params = request.params();
    BOOST_LOG_SEV(my_logger::get(), INFO) << "Received following parameters: ";
    BOOST_LOG_SEV(my_logger::get(), INFO) << "params['template_id']: " << params["template_id"];
    BOOST_LOG_SEV(my_logger::get(), INFO) << "params['top_text']: " << params["top_text"];
    BOOST_LOG_SEV(my_logger::get(), INFO) << "params['bottom_text']: " << params["bottom_text"];
    // call createMeme() with given parameters
    std::map<std::string, std::string> new_meme = createMeme(std::atoi(params["template_id"].c_str()), params["top_text"], params["bottom_text"]);

    std::string new_uri = "/meme/view?id=" + new_meme["meme_id"];

    auto req = request;
    req.set_uri(new_uri);

    return HandleRequest(req);
  }
  else if (action_str == "view") {
    BOOST_LOG_SEV(my_logger::get(), INFO) << "MemeHandler::HandleRequest - handling /meme/view";
    // parse params from params_str
    size_t pos = params_str.find("=");
    while (pos != std::string::npos) {
      std::string key = params_str.substr(0, pos);
      params_str = params_str.substr(pos+1);

      std::string value;
      pos = params_str.find("&");
      if (pos != std::string::npos) {
        value = params_str.substr(0, pos);
        params_str = params_str.substr(pos+1);
      }
      else
        value = params_str;

      params[key] = value;
      pos = params_str.find("=");
    }

    // call viewMeme() with given parameters
    std::map<std::string, std::string> meme_map = viewMeme(std::atoi(params["id"].c_str()));
    HtmlBuilder partialBuilder("./static/memes/partials/_meme.html");
    HtmlBuilder showBuilder("./static/memes/show.html");
    partialBuilder.inject(meme_map);
    showBuilder.inject("meme", partialBuilder.getHtml());
    reply_ptr->SetBody(showBuilder.getHtml());
    reply_ptr->SetHeader("Content-Type", "text/html");
  }
  else if (action_str == "list") {
    BOOST_LOG_SEV(my_logger::get(), INFO) << "MemeHandler::HandleRequest - handling /meme/list";
    std::vector<std::map<std::string, std::string>> memes = viewMemes();
    HtmlBuilder partialBuilder("./static/memes/partials/_meme.html");
    HtmlBuilder indexBuilder("./static/memes/index.html");
    partialBuilder.inject(memes);
    indexBuilder.inject("memes", partialBuilder.getHtml());
    reply_ptr->SetBody(indexBuilder.getHtml());
    reply_ptr->SetHeader("Content-Type", "text/html");
  }
  else if (action_str == "assets") {
    BOOST_LOG_SEV(my_logger::get(), INFO) << "MemeHandler::HandleRequest - handling /meme/assets";
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
    reply_ptr->SetBody(file_content);
    reply_ptr->SetHeader("Content-Type", GetContentType(file_path));
  }

  reply_ptr->SetStatus(200);
  return reply_ptr;
}

/**
      return the form for users to create a meme

      @return the content of the new page as a string
*/
std::string MemeHandler::newMeme(){

  //TODO: implement the string contents later
  std::string form_string;
  form_string = "";

  return form_string;
}

/**
    given the parameters passed from the form submitted by the user
    create a row in the table in our database
    after creating a row in database, call viewMeme() with the new
    meme id

    @param template_id template id chosen by the user
    @param top_text top text submitted by the user
    @param bottom_text bottom text submitted by the user
    @return the return value from viewMeme(meme_id) where meme_id is the
            new meme_id
*/
std::map<std::string, std::string> MemeHandler::createMeme(const uint32_t template_id,
                                                 std::string& top_text,
                                                 std::string& bottom_text){
  uint32_t meme_id = database->AddMeme(template_id, top_text, bottom_text);
  // TODO: fix this meme_id to get the meme_id from a newly created meme
  // uint32_t meme_id = 0;

  // TODO: implement redirection to meme/view after redirection
  return viewMeme(meme_id);
}

/**
   given a meme id, get the meme id, template image, top text and bottom text
   from the database and return them as a vector of strings

   @param meme_id meme id passed in URL
   @return a vetcor with meme id, template image, top text, and bottom text
*/
std::map<std::string, std::string> MemeHandler::viewMeme(const uint32_t meme_id){
  Meme meme = database->GetMeme(meme_id);
  std::map<std::string, std::string> meme_map;
  meme_map["meme_id"] = std::to_string(meme.meme_id);
  meme_map["template_id"] = std::to_string(meme.template_id);
  meme_map["top_text"] = meme.top_text;
  meme_map["bottom_text"] = meme.bottom_text;

  return meme_map;
}

std::vector<std::map<std::string, std::string>> MemeHandler::viewMemes() {
  std::vector<Meme> all_meme = database->GetAllMemes();
  std::vector<std::map<std::string, std::string>> memes;

  for (auto &meme : all_meme) {
    std::map<std::string, std::string> meme_map;
    meme_map["meme_id"] = std::to_string(meme.meme_id);
    meme_map["template_id"] = std::to_string(meme.template_id);
    meme_map["top_text"] = meme.top_text;
    meme_map["bottom_text"] = meme.bottom_text;
    memes.push_back(meme_map);
  }

  return memes;
}

/**
   retrieve all memes from the database and return the list of meme ids

   @return a vector of meme ids
*/
std::vector<uint32_t> MemeHandler::listMeme(){
  std::vector<Meme> all_meme = database->GetAllMemes();
  std::vector<uint32_t> all_meme_id;

  for (std::vector<Meme>::iterator it = all_meme.begin(); it!= all_meme.end(); it++)
  {
    all_meme_id.push_back((*it).meme_id);
  }

  return all_meme_id;
}

std::string MemeHandler::GetPath(std::string url) {
  // We assume the url has the prefix declared in the StaticHandler
  int prefix_len = this->uri_prefix.length();
  std::string path =
      this->root_path + this->path_prefix + url.substr(prefix_len);

  return path;
}

bool MemeHandler::IsRegularFile(std::string path) {
  // Check if requested path is a regular file
  struct stat path_stat;

  stat(path.c_str(), &path_stat);
  if (!S_ISREG(path_stat.st_mode)) {
    return false;
  }

  return true;
}
std::string MemeHandler::GetContentType(std::string file_name) {
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

std::string MemeHandler::GetContent(std::ifstream& file) {
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

std::string MemeHandler::ReadFromFile(std::string file_path) {
  // std::string file_path = GetPath(request.uri());
  std::ifstream file;
  std::string empty_str = "";

  if (!IsRegularFile(file_path)) {
    std::cerr << "Error: Static File requested, but file is not regular"
              << std::endl;
    BOOST_LOG_SEV(my_logger::get(), ERROR)
        << "Static File requested, but file is not regular";
    return empty_str;
  }

  file.open(file_path);
  if (!file.is_open()) {
    std::cerr << "Error: File could not be opened" << std::endl;
    BOOST_LOG_SEV(my_logger::get(), ERROR) << "File could not be opened";
    return empty_str;
  }

  std::string file_content = GetContent(file);

  return file_content;
}
