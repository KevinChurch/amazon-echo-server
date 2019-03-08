#include "html_builder.h"
#include <fstream>
#include <streambuf>
#include <vector>
#include "gtest/gtest.h"

class HtmlBuilderTest : public ::testing::Test {
 protected:
};

TEST_F(HtmlBuilderTest, Contructor) {
  HtmlBuilder htmlBuilder("./html/before1.html");
  std::ifstream file("./html/before1.html");
  std::string html = std::string((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());

  EXPECT_EQ(htmlBuilder.getHtml(), html);
}

TEST_F(HtmlBuilderTest, InjectAttribute) {
  HtmlBuilder htmlBuilder("./html/before1.html");
  std::ifstream file("./html/after1.html");
  std::string html = std::string((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());

  htmlBuilder.inject("REPLACE ME 1", "Amazon");
  htmlBuilder.inject("REPLACE ME 2", "Welcome!");

  EXPECT_EQ(htmlBuilder.getHtml(), html);
}

TEST_F(HtmlBuilderTest, InjectAttributes) {
  HtmlBuilder htmlBuilder("./html/before1.html");
  std::ifstream file("./html/after1.html");
  std::string html = std::string((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());

  std::vector<std::string> originals = {"REPLACE ME 1", "REPLACE ME 2"};
  std::vector<std::string> replacements = {"Amazon", "Welcome!"};

  htmlBuilder.inject(originals, replacements);

  EXPECT_EQ(htmlBuilder.getHtml(), html);
}

TEST_F(HtmlBuilderTest, MultipleInjectAttributes) {
  HtmlBuilder htmlBuilder1("./html/partials/_meme.html");
  HtmlBuilder htmlBuilder2("./html/before2.html");
  std::ifstream file("./html/after2.html");
  std::string html = std::string((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());

  std::vector<std::string> originals = {"meme.meme_id", "meme.template_id",
                                        "meme.top_text", "meme.bottom_text"};
  std::vector<std::vector<std::string>> replacements = {
      {"1", "1", "top text 1", "bottom text 1"},
      {"2", "2", "top text 2", "bottom text 2"},
      {"3", "1", "top text 3", "bottom text 3"},
      {"4", "2", "top text 4", "bottom text 4"},
      {"5", "1", "top text 5", "bottom text 5"}};

  htmlBuilder1.inject(originals, replacements);
  htmlBuilder2.inject("memes", htmlBuilder1.getHtml());

  EXPECT_EQ(htmlBuilder2.getHtml(), html);
}