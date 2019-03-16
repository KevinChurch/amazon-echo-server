#pragma once

#include <map>
#include <regex>
#include <string>
#include <vector>

class HtmlBuilder {
 public:
  HtmlBuilder();
  HtmlBuilder(std::string filename);

  std::string getHtml();

  void inject(std::string original, std::string replacement);
  void inject(std::vector<std::string> originals,
              std::vector<std::string> replacements);
  void inject(std::map<std::string, std::string> input);
  void inject(std::vector<std::map<std::string, std::string>> input);
  void inject(std::vector<std::string> originals,
              std::vector<std::vector<std::string>> replacements);
  void equalityInject(std::string value, std::string prefix,
                      std::string replacement);

 private:
  std::string html;

  void shallowInject(std::string &tHtml, std::string original,
                     std::string replacement);
  void shallowInject(std::string &tHtml, std::vector<std::string> originals,
                     std::vector<std::string> replacements);
  void shallowInject(std::string &tHtml,
                     std::map<std::string, std::string> input);
  std::string generateMarker(std::string input);
};
