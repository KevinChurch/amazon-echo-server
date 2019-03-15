#pragma once

#include "handler.h"
#include "reply.h"
#include "request.h"
#include "database.h"
#include <string>
#include <vector>

/**
    Handler for echoing requests.
*/
class MemeHandler : public Handler {
public:
  /**
      Factory method for creating an EchoHandler pointer.

      @param config the config block for an EchoHandler.
      @param root_path the root_path for an EchoHandler.
      @return a EchoHandler pointer.
  */
  static MemeHandler* create(const NginxConfig& config,
                             const std::string& root_path);
  /**
      Handles a request.

      @param request the config block for an EchoHandler.
      @return a Reply unique pointer.
  */
  std::unique_ptr<Reply> HandleRequest(const Request& request);
  /**
      Given a config block and the root path of the server, initialize data members

      @param config config block of requested handler
      @param root_path root path of the server
      @return true if succeeed, false otherwise
  */
  bool Init(const NginxConfig& config, const std::string& root_path);
  std::unique_ptr<Database> InitDatabase();

private:
  std::unique_ptr<Database> database;
  // uri prefix for request handler, equivalent to root in handler blocks in config
  std::string uri_prefix;
  // path prefix for requested handler, equivalent to root in handler blocks in config
  std::string path_prefix;
  // path of a server root
  std::string root_path;

  /**
      return the form for users to create a meme

      @return the content of the new page as a string
  */
  std::string newMeme();

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
  std::map<std::string, std::string> createMeme(std::string& template_id,
                                                   std::string& top_text,
                                                   std::string& bottom_text);

  std::map<std::string, std::string> editMeme(std::string& meme_id,
                                                          std::string& template_id,
                                                          std::string& top_text,
                                                          std::string& bottom_text);

  /**
     given a meme id, get the meme id, template image, top text and bottom text
     from the database and return them as a vector of strings

     @param meme_id meme id passed in URL
     @return a vetcor with meme id, template image, top text, and bottom text
  */
  std::map<std::string, std::string> viewMeme(const uint32_t meme_id);
  std::map<std::string, std::string> viewMeme(const std::string meme_id_str);

  std::vector<std::map<std::string, std::string>> viewMemes();

  static bool compareMemesId(std::map<std::string, std::string> meme1, std::map<std::string, std::string> meme2);

  std::vector<std::map<std::string, std::string>> findMemes(std::string search_param);

  /**
     retrieve all memes from the database and return the list of meme ids

     @return a vector of meme ids
  */
  std::vector<uint32_t> listMeme();

  int deleteMeme(const uint32_t meme_id);

  /**
      Given a url received from request, get a file path to a static file
      requested by client

      @param url the url received from request
      @return path to the requested static file
  */
  std::string GetPath(std::string url);
  /**
      Given a file paht, determine if the file is a regular file

      @param path path to the requested static file
      @return true if regular file, false if not a regular file
  */
  bool IsRegularFile(std::string path);
  /**
      Given a file_name, get the file extension to determing the content type

      @param file_name file name
      @return content type
  */
  std::string GetContentType(std::string file_name);
  /**
      Given a file, get the content of the file

      @param file file to get content from
      @return content of the file
  */
  std::string GetContent(std::ifstream& file);
  std::string ReadFromFile(std::string file_path);
  std::map<std::string, std::string> GetParams(std::string params_str);
  std::string pseudoDecode(std::string &string);
};
