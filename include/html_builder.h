#pragma once

#include <string>

class HtmlBuilder {
 public:
  HtmlBuilder();
  HtmlBuilder(std::string filename);

  bool inject(std::string string1, std::string string2);

  std::string getHtml();

 private:
  std::string html;
};
