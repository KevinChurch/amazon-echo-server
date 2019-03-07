#include "meme_handler.h"
#include "database.h"

MemeHandler* MemeHandler::create(const NginxConfig& config,
                                     const std::string& root_path) {
  return new MemeHandler;
}



std::unique_ptr<Reply> MemeHandler::HandleRequest(const Request& request) {
  std::cout << "\nMemeHandler::HandleRequest" << std::endl;
  BOOST_LOG_SEV(my_logger::get(), INFO) << "\nMemeHandler::HandleRequest";

  std::unique_ptr<Reply> reply_ptr(new Reply());  
  reply_ptr->SetStatus(200);
  reply_ptr->SetHeader("Content-Type", "text/html");
  reply_ptr->SetBody(newMeme);
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
  std::vector<std::string> MemeHandler::createMeme(const uint8_t template_id,
                                      std::string& top_text,
                                      std::string& bottom_text){
    database db;
    db.AddMeme(template_id, top_text, bottom_text);

    //TODO: implement meme_id later

    return viewMeme(meme_id);

  }

  /**
     given a meme id, get the meme id, template image, top text and bottom text
     from the database and return them as a vector of strings

     @param meme_id meme id passed in URL
     @return a vetcor with meme id, template image, top text, and bottom text
  */
  std::vector<std::string> MemeHandler::viewMeme(const uint8_t meme_id){
    database db;
    Meme* meme = db.GetMeme(meme_id);
    std::vector<std::string> meme_vector;
    meme_vector.push_back(meme->meme_id);
    meme_vector.push_back(meme->template_id);
    meme_vector.push_back(meme->top_text);
    meme_vector.push_back(meme->bottom_text);
    return meme_vector;
  }

  /**
     retrieve all memes from the database and return the list of meme ids

     @return a vector of meme ids
  */
  std::vector<uint8_t> MemeHandler::listMeme(){
    database db;
    std::vector<Meme*> all_meme = db.GetAllMemes();
    std::vector<uint8_t> all_meme_id;
    
    for (std::vector<Meme*>::iterator it = all_meme.begin(); it!= all_meme.end(); it++)
    {
      all_meme_id.push_back(*it->meme_id);
    }
    return all_meme_id;
  }





