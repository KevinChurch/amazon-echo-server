#pragma once
#include <iostream>
#include <string>
#include <sstream>

struct Meme {
  uint32_t meme_id;
  uint32_t template_id;
  std::string top_text;
  std::string bottom_text;

  std::string toString() {
    std::ostringstream oss;
    oss <<  "meme => {meme_id: " << meme_id << ", " <<
    "template_id: " << template_id << ", " <<
    "top_text: " << top_text << ", " <<
    "bottom_text: " << bottom_text << "}";
    return oss.str();
  }
};
