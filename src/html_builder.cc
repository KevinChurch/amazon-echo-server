#include "html_builder.h"
#include <fstream>
#include <streambuf>

HtmlBuilder::HtmlBuilder() {}

HtmlBuilder::HtmlBuilder(std::string file_path) {
  std::ifstream file(file_path);
  html = std::string((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
}

HtmlBuilder::~HtmlBuilder() {}

std::string HtmlBuilder::getHtml() { return html; }

bool HtmlBuilder::inject(std::string original, std::string replacement) {
    size_t pos = html.find(original);
    bool result = (pos != std::string::npos);
    if (result) {
        html.replace(pos, original.length(), replacement);
    }
    return result;
}