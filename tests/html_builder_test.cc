#include "html_builder.h"
#include <fstream>
#include <map>
#include <streambuf>
#include <vector>
#include "gtest/gtest.h"
#include "regex"

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

TEST_F(HtmlBuilderTest, InjectAttributeWithDuplicates) {
  HtmlBuilder htmlBuilder("./html/before4.html");
  std::ifstream file("./html/after4.html");
  std::string html = std::string((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());

  htmlBuilder.inject("test", "test");
  EXPECT_EQ(htmlBuilder.getHtml(), html);
}

TEST_F(HtmlBuilderTest, InjectAttributesWithVectors) {
  HtmlBuilder htmlBuilder("./html/before1.html");
  std::ifstream file("./html/after1.html");
  std::string html = std::string((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());

  std::vector<std::string> originals = {"REPLACE ME 1", "REPLACE ME 2"};
  std::vector<std::string> replacements = {"Amazon", "Welcome!"};

  htmlBuilder.inject(originals, replacements);

  EXPECT_EQ(htmlBuilder.getHtml(), html);
}

TEST_F(HtmlBuilderTest, InjectAttributesWithMaps) {
  HtmlBuilder htmlBuilder("./html/before1.html");
  std::ifstream file("./html/after1.html");
  std::string html = std::string((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());

  std::map<std::string, std::string> input = {{"REPLACE ME 1", "Amazon"},
                                              {"REPLACE ME 2", "Welcome!"}};

  htmlBuilder.inject(input);

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

TEST_F(HtmlBuilderTest, MultipleInjectAttributesWithMaps) {
  HtmlBuilder htmlBuilder1("./html/partials/_meme.html");
  HtmlBuilder htmlBuilder2("./html/before2.html");
  std::ifstream file("./html/after2.html");
  std::string html = std::string((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());

  std::vector<std::map<std::string, std::string>> inputs = {
      {{"meme.meme_id", "1"},
       {"meme.template_id", "1"},
       {"meme.top_text", "top text 1"},
       {"meme.bottom_text", "bottom text 1"}},
      {{"meme.meme_id", "2"},
       {"meme.template_id", "2"},
       {"meme.top_text", "top text 2"},
       {"meme.bottom_text", "bottom text 2"}},
      {{"meme.meme_id", "3"},
       {"meme.template_id", "1"},
       {"meme.top_text", "top text 3"},
       {"meme.bottom_text", "bottom text 3"}},
      {{"meme.meme_id", "4"},
       {"meme.template_id", "2"},
       {"meme.top_text", "top text 4"},
       {"meme.bottom_text", "bottom text 4"}},
      {{"meme.meme_id", "5"},
       {"meme.template_id", "1"},
       {"meme.top_text", "top text 5"},
       {"meme.bottom_text", "bottom text 5"}}};

  htmlBuilder1.inject(inputs);
  htmlBuilder2.inject("memes", htmlBuilder1.getHtml());

  EXPECT_EQ(htmlBuilder2.getHtml(), html);
}

TEST_F(HtmlBuilderTest, EqualityInject) {
  HtmlBuilder htmlBuilder("./html/before3.html");
  std::ifstream file("./html/after3.html");
  std::string html = std::string((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());

  std::map<std::string, std::string> input({{"meme_id", "1"},
                                            {"top_text", "top text"},
                                            {"bottom_text", "bottom text"}});

  htmlBuilder.inject(input);
  htmlBuilder.equalityInject("2", "template_id", "selected");

  EXPECT_EQ(htmlBuilder.getHtml(), html);
}