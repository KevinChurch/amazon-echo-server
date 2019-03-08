#include "html_builder.h"
#include <fstream>
#include <streambuf>
#include "gtest/gtest.h"

class HtmlBuilderTest : public ::testing::Test {
 protected:
  HtmlBuilder htmlBuilder;
};

TEST_F(HtmlBuilderTest, Contructor) {
  htmlBuilder = HtmlBuilder("./html/before.html");
  std::ifstream file("./html/before.html");
  std::string html = std::string((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());

  EXPECT_EQ(htmlBuilder.getHtml(), html);
}

TEST_F(HtmlBuilderTest, Inject) {
  htmlBuilder = HtmlBuilder("./html/before.html");
  std::ifstream file("./html/after.html");
  std::string html = std::string((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());

  htmlBuilder.inject("<!-- REPLACE ME 1 -->", "Amazon");
  htmlBuilder.inject("<!-- REPLACE ME 2 -->", "Welcome!");

  EXPECT_EQ(htmlBuilder.getHtml(), html);
}