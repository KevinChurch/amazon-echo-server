#include "html_builder.h"
#include <fstream>
#include <iostream>
#include <streambuf>

HtmlBuilder::HtmlBuilder() {}

HtmlBuilder::HtmlBuilder(std::string file_path) {
  std::ifstream file(file_path);
  html = std::string((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
}

std::string HtmlBuilder::getHtml() { return html; }

void HtmlBuilder::inject(std::string original, std::string replacement) {
  std::string marker = generateMarker(original);

  size_t pos = html.find(marker);
  if (pos != std::string::npos) {
    html.replace(pos, marker.length(), replacement);
  }
}

void HtmlBuilder::inject(std::vector<std::string> originals,
                         std::vector<std::string> replacements) {
  if (originals.size() == replacements.size()) {
    int length = originals.size();
    for (int i = 0; i < length; ++i) {
      inject(originals[i], replacements[i]);
    }
  }
}

void HtmlBuilder::inject(std::map<std::string, std::string> input) {
  for (std::map<std::string, std::string>::iterator it = input.begin(); it != input.end(); ++it) {
    inject(it->first, it->second);
  }
}

void HtmlBuilder::inject(std::vector<std::map<std::string, std::string>> input) {
  std::string tHtmls;
  for (std::vector<std::map<std::string, std::string>>::iterator it = input.begin(); it != input.end(); ++it) {
    std::string tHtml = html;
    shallowInject(tHtml, *it);
    if (it != input.begin()) {
      tHtml = "\n" + tHtml;
    }
    tHtmls.append(tHtml);
  }
  html = tHtmls;
}

void HtmlBuilder::inject(std::vector<std::string> originals,
                         std::vector<std::vector<std::string>> replacements) {
  std::string tHtmls;
  if (!replacements.empty() && (originals.size() == replacements[0].size())) {
    int length = replacements.size();
    for (int i = 0; i < length; ++i) {
      std::string tHtml = html;
      shallowInject(tHtml, originals, replacements[i]);
      if (i) {
        tHtml = "\n" + tHtml;
      }
      tHtmls.append(tHtml);
    }
  }
  html = tHtmls;
}

void HtmlBuilder::shallowInject(std::string &tHtml, std::string original,
                                std::string replacement) {
  std::string marker = generateMarker(original);

  size_t pos = tHtml.find(marker);
  if (pos != std::string::npos) {
    tHtml.replace(pos, marker.length(), replacement);
  }
}

void HtmlBuilder::shallowInject(std::string &tHtml, std::map<std::string, std::string> input) {
  for (std::map<std::string, std::string>::iterator it = input.begin(); it != input.end(); ++it) {
    shallowInject(tHtml, it->first, it->second);
  }
}

void HtmlBuilder::shallowInject(std::string &tHtml,
                                std::vector<std::string> originals,
                                std::vector<std::string> replacements) {
  if (originals.size() == replacements.size()) {
    int length = originals.size();
    for (int i = 0; i < length; ++i) {
      shallowInject(tHtml, originals[i], replacements[i]);
    }
  }
}

std::string HtmlBuilder::generateMarker(std::string input) {
  return "<!-- " + input + " -->";
}