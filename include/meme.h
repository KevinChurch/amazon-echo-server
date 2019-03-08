#pragma once
#include <string>

struct Meme {
  uint32_t meme_id;
  uint32_t template_id;
  std::string top_text;
  std::string bottom_text;
};
