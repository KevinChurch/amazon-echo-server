#pragma once
#include "meme.h"

class Database {
 public:
  
  Meme* GetMeme(uint8_t id) const;
  std::vecotr<Meme*> GetAllMemes() const;

  uint8_t AddMeme(uint8_t template_id, std::string top_text, std::string bottom_text);

 private:

};
