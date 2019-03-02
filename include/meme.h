#pragma once
#include <string>

struct Meme {
  uint8_t meme_id;
  uint8_t template_id;
  std::string top_text;
  std::string bottom_text;
};
