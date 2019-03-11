#pragma once
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <sqlite3.h>
#include <vector>
#include "meme.h"

class Database {
 public:

  //Initialization function to be run on startup
  void Init(void);

  //Search/Get Meme functions
  Meme GetMeme(uint32_t id) const;
  std::vector<Meme> FindMemes(std::string search_string);
  std::vector<Meme> GetAllMemes() const;

  //Insert Meme function
  int AddMeme(uint32_t template_id, std::string top_text, std::string bottom_text);

  //Update Meme function
  int UpdateMeme(uint32_t meme_id, uint32_t template_id, std::string top_text, std::string bottom_text);

  //Delete Meme function
  int DeleteMeme(uint32_t meme_id);

 private:

};
