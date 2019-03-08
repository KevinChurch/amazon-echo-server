#pragma once

#include <string>
#include <vector>

class HtmlBuilder {
 public:
  HtmlBuilder();
  HtmlBuilder(std::string filename);

  void inject(std::string original, std::string replacement);
  void inject(std::vector<std::string> originals,
              std::vector<std::string> replacements);
  void inject(std::vector<std::string> originals,
              std::vector<std::vector<std::string>> replacements);

  std::string getHtml();

 private:
  std::string html;

  void shallowInject(std::string &tHtml, std::string original,
                     std::string replacement);
  void shallowInject(std::string &tHtml, std::vector<std::string> originals,
                     std::vector<std::string> replacements);
  std::string generateMarker(std::string input);
};
