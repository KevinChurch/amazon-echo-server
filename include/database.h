#pragma once
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <sqlite3.h>
#include <vector>
#include "meme.h"

class Database {
 public:
  
  void Init(void);
  Meme GetMeme(uint32_t id) const;
  std::vector<Meme> GetAllMemes() const;

  uint32_t AddMeme(uint32_t template_id, std::string top_text, std::string bottom_text);

 private:

};
