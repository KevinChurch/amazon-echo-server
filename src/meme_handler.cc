#include "meme_handler.h"
#include "database.h"
#include <vector>
#include "html_builder.h"
#include <boost/algorithm/string.hpp>

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
  BOOST_LOG_SEV(my_logger::get(), INFO) << "\nMemeHandler::HandleRequest";
  BOOST_LOG_SEV(my_logger::get(), INFO) << "::ResponseMetrics:: Handler Name: MemeHandler";
  BOOST_LOG_SEV(my_logger::get(), INFO) << "::ResponseMetrics:: Request Path: " << request.uri();
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

  // /meme/new
  if (action_str == "new") {
    BOOST_LOG_SEV(my_logger::get(), INFO) << "MemeHandler::HandleRequest - handling /meme/new";
    params = GetParams(params_str);
    BOOST_LOG_SEV(my_logger::get(), INFO) << "Received following valid parameters: ";
    BOOST_LOG_SEV(my_logger::get(), INFO) << "params['update']: " << params["update"];

    if (params["update"] != "") { // update parameter is not empty
      // get the meme with the meme_id passed in params
      std::map<std::string, std::string> meme_map = viewMeme(params["update"]);

      // check if the meme exists for given meme_id
      if (meme_map["meme_id"] == "") { // meme doesn't exist for given id - return null pointer
        BOOST_LOG_SEV(my_logger::get(), INFO) << "Not a valid meme ID for update";
        return nullptr;
      }
      else { // meme exists for given id
        HtmlBuilder editBuilder("./static/memes/edit.html");
        editBuilder.inject(meme_map);
        std::string value = meme_map.find("template_id")->second;
        editBuilder.equalityInject(value, "template_id", "selected");
        reply_ptr->SetBody(editBuilder.getHtml());
        reply_ptr->SetHeader("Content-Type", "text/html");
      }
    }
    else { // update parameter is empty
      // get content of /static/memes/assets/new.html
      file_path = "./static/memes/new.html";
      body = ReadFromFile(file_path);
      // set the body
      reply_ptr->SetBody(body);
      reply_ptr->SetHeader("Content-Type", "text/html");
    }
  }
  // /meme/create
  else if (action_str == "create") {
    BOOST_LOG_SEV(my_logger::get(), INFO) << "MemeHandler::HandleRequest - handling /meme/create";
    
    // check if the method is POST/PUT
    if (!boost::iequals(request.method(), "POST")) {
      BOOST_LOG_SEV(my_logger::get(), ERROR) << "Not a valid method for request to /meme/create";
      return nullptr;
    }
    // get params from request
    params = request.params();
    BOOST_LOG_SEV(my_logger::get(), INFO) << "Received following valid parameters: ";
    BOOST_LOG_SEV(my_logger::get(), INFO) << "params['template_id']: " << params["template_id"];
    BOOST_LOG_SEV(my_logger::get(), INFO) << "params['top_text']: " << params["top_text"];
    BOOST_LOG_SEV(my_logger::get(), INFO) << "params['bottom_text']: " << params["bottom_text"];
    BOOST_LOG_SEV(my_logger::get(), INFO) << "params['update']: " << params["update"];
    // call createMeme() with given parameters
    std::string top_text = pseudoDecode(params["top_text"]);
    std::string bottom_text = pseudoDecode(params["bottom_text"]);

    std::map<std::string, std::string> new_meme;
    if (params["update"] != "") { // update parameter is not empty
      new_meme = editMeme(params["update"], params["template_id"], top_text, bottom_text);
    }
    else {
      new_meme = createMeme(params["template_id"], top_text, bottom_text);
      if (new_meme.empty()) {
        BOOST_LOG_SEV(my_logger::get(), ERROR) << "MemeHandler::HandleRequest - meme creation failure: error with creating a meme";
        return nullptr;
      }
    }

    // redirect version
    std::string new_location = "/meme/view?id=" + new_meme["meme_id"];

    BOOST_LOG_SEV(my_logger::get(), INFO) << "::ResponseMetrics:: Response Code: 301";
    reply_ptr->SetStatus(301);
    reply_ptr->SetHeader("Location", new_location);
    return reply_ptr;
  }
  // /meme/view
  else if (action_str == "view") {
    BOOST_LOG_SEV(my_logger::get(), INFO) << "MemeHandler::HandleRequest - handling /meme/view";
    params = GetParams(params_str);
    BOOST_LOG_SEV(my_logger::get(), INFO) << "Received following valid parameters: ";
    BOOST_LOG_SEV(my_logger::get(), INFO) << "params['id']: " << params["id"];

    // call viewMeme() with given parameters
    std::map<std::string, std::string> meme_map = viewMeme(params["id"]);
    HtmlBuilder partialBuilder("./static/memes/partials/_meme.html");
    HtmlBuilder showBuilder("./static/memes/show.html");
    partialBuilder.inject(meme_map);
    showBuilder.inject("meme", partialBuilder.getHtml());
    reply_ptr->SetBody(showBuilder.getHtml());
    reply_ptr->SetHeader("Content-Type", "text/html");
  }
  // /meme/list
  else if (action_str == "list") {
    BOOST_LOG_SEV(my_logger::get(), INFO) << "MemeHandler::HandleRequest - handling /meme/list";
    params = GetParams(params_str);
    std::vector<std::map<std::string, std::string>> memes;

    if (params["q"] != "") {
      memes = findMemes(params["q"]);
    }
    else {
      memes = viewMemes();
    }

    // std::vector<std::map<std::string, std::string>> memes = viewMemes();
    HtmlBuilder partialBuilder("./static/memes/partials/_meme.html");
    HtmlBuilder indexBuilder("./static/memes/index.html");
    partialBuilder.inject(memes);
    indexBuilder.inject("memes", partialBuilder.getHtml());
    reply_ptr->SetBody(indexBuilder.getHtml());
    reply_ptr->SetHeader("Content-Type", "text/html");
  }
  // /meme/assets
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
  // /meme/delete
  else if(action_str == "delete") {
    BOOST_LOG_SEV(my_logger::get(), INFO) << "MemeHandler::HandleRequest - handling /meme/delete";
    params = GetParams(params_str);
    BOOST_LOG_SEV(my_logger::get(), INFO) << "Received following valid parameters: ";
    BOOST_LOG_SEV(my_logger::get(), INFO) << "params['id']: " << params["id"];

    // call deleteMeme() with given parameters
    int result = deleteMeme(std::atoi(params["id"].c_str()));
    if (result == -1) {
      BOOST_LOG_SEV(my_logger::get(), ERROR) << "MemeHandler::HandleRequest - deleteMeme failed";
    }

    std::string new_location = "/meme/list";

    BOOST_LOG_SEV(my_logger::get(), INFO) << "::ResponseMetrics:: Response Code: 301";
    reply_ptr->SetStatus(301);
    reply_ptr->SetHeader("Location", new_location);
    return reply_ptr;
  }
  // other invalid URL's
  else {
    return nullptr;
  }

  BOOST_LOG_SEV(my_logger::get(), INFO) << "::ResponseMetrics:: Response Code: 200";
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
std::map<std::string, std::string> MemeHandler::createMeme(std::string& template_id,
                                                 std::string& top_text,
                                                 std::string& bottom_text){
  std::map<std::string, std::string> meme;
  uint32_t uint_template_id = std::atoi(template_id.c_str());
  int meme_id = database->AddMeme(uint_template_id, top_text, bottom_text);
  // if database->AddMeme return -1, it means it failed, so return an empty map
  if (meme_id == -1) {
    return meme;
  }

  meme = viewMeme(meme_id);
  return meme;
}

std::map<std::string, std::string> MemeHandler::editMeme(std::string& meme_id,
                                                         std::string& template_id,
                                                         std::string& top_text,
                                                         std::string& bottom_text){
  std::map<std::string, std::string> meme;
  uint32_t uint_meme_id = std::atoi(meme_id.c_str());
  uint32_t uint_template_id = std::atoi(template_id.c_str());
  int new_meme_id = database->UpdateMeme(uint_meme_id, uint_template_id, top_text, bottom_text);
  // if database->UpdateMeme return -1, it means it failed, so return an empty map
  if (new_meme_id == -1) {
    return meme;
  }

  meme = viewMeme(meme_id);
  return meme;
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

std::map<std::string, std::string> MemeHandler::viewMeme(const std::string meme_id_str){
  uint32_t meme_id = std::atoi(meme_id_str.c_str());
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

bool MemeHandler::compareMemesId(std::map<std::string, std::string> meme1, std::map<std::string, std::string> meme2) {
    return (meme1["meme_id"] > meme2["meme_id"]);
}

std::vector<std::map<std::string, std::string>> MemeHandler::findMemes(std::string search_param) {
  std::string search_string = pseudoDecode(search_param);
  std::vector<Meme> all_meme = database->FindMemes(search_string);
  std::vector<std::map<std::string, std::string>> memes;

  for (auto &meme : all_meme) {
    std::map<std::string, std::string> meme_map;
    meme_map["meme_id"] = std::to_string(meme.meme_id);
    meme_map["template_id"] = std::to_string(meme.template_id);
    meme_map["top_text"] = meme.top_text;
    meme_map["bottom_text"] = meme.bottom_text;
    memes.push_back(meme_map);
  }

  sort(memes.begin(), memes.end(), compareMemesId);

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

int MemeHandler::deleteMeme(const uint32_t meme_id) {
  return database->DeleteMeme(meme_id);
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


std::string MemeHandler::pseudoDecode(std::string &string) {
  std::string result = string;

  size_t pos = result.find("+");
  while (pos != std::string::npos) {
    result.replace(pos, 1, " ");
    pos = result.find("+");
  }
  return result;
}

std::map<std::string, std::string> MemeHandler::GetParams(std::string params_str) {
  std::map<std::string, std::string> params;

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

  return params;
}
