#pragma once

#include "handler.h"
#include "reply.h"
#include "request.h"
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

private:
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
  std::vector<std::string> createMeme(const uint8_t template_id,
                                      std::string& top_text,
                                      std::string& bottom_text);

  /**
     given a meme id, get the meme id, template image, top text and bottom text
     from the database and return them as a vector of strings

     @param meme_id meme id passed in URL
     @return a vetcor with meme id, template image, top text, and bottom text
  */
  std::vector<std::string> viewMeme(const uint8_t meme_id);

  /**
     retrieve all memes from the database and return the list of meme ids

     @return a vector of meme ids
  */
  std::vector<unit8_t> listMeme();
};
