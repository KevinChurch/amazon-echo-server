#include "gtest/gtest.h"
#include "config_parser.h"
#include "config_parser_exception.h"

class NginxConfigParserTest : public ::testing::Test {
protected:
  NginxConfigParser parser;
  NginxConfig out_config;
  bool success;
};

TEST_F(NginxConfigParserTest, SimpleConfigFile) {
  success = parser.Parse("example_config", &out_config);

  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, EmptyConfigFile) {
  success = parser.Parse("example_empty_config", &out_config);

  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, CommentConfigFile) {
  success = parser.Parse("example_comment_config", &out_config);

  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, ToString) {
  parser.Parse("example_config", &out_config);

  EXPECT_EQ(out_config.ToString(0), "\
port 80;\n\
root ./;\n\
handler static {\n\
  location /static;\n\
  root static;\n\
  name static;\n\
}\n\
handler static {\n\
  location /static2;\n\
  root /www/data;\n\
  name static;\n\
}\n\
handler echo {\n\
  location /echo;\n\
  name echo;\n\
}\n\
handler not_found {\n\
  location /;\n\
  name not_found;\n\
}\n\
handler status {\n\
  location /status;\n\
  name status;\n\
}\n");

  EXPECT_EQ(out_config.ToString(1), "\
  port 80;\n\
  root ./;\n\
  handler static {\n\
    location /static;\n\
    root static;\n\
    name static;\n\
  }\n\
  handler static {\n\
    location /static2;\n\
    root /www/data;\n\
    name static;\n\
  }\n\
  handler echo {\n\
    location /echo;\n\
    name echo;\n\
  }\n\
  handler not_found {\n\
    location /;\n\
    name not_found;\n\
  }\n\
  handler status {\n\
    location /status;\n\
    name status;\n\
  }\n");
}

TEST_F(NginxConfigParserTest, Find) {
  parser.Parse("example_config", &out_config);

  // Find with String
  EXPECT_EQ(out_config.Find("port"), "80");
  EXPECT_EQ(out_config.Find("root"), "./");

  EXPECT_NE(out_config.Find("port"), "");
  EXPECT_NE(out_config.Find("root"), "");

  // Find with Vector
  std::vector<std::string> vect1 = {"port"};
  EXPECT_EQ(out_config.Find(vect1), "80");
  std::vector<std::string> vect2 = {"root"};
  EXPECT_EQ(out_config.Find(vect2), "./");
  std::vector<std::string> vect3 = {"bad"};
  EXPECT_EQ(out_config.Find(vect3), "");
  std::vector<std::string> vect4 = {"bad", "bad"};
  EXPECT_EQ(out_config.Find(vect4), "");

  EXPECT_NE(out_config.Find(vect1), "");
  EXPECT_NE(out_config.Find(vect2), "");

  // Find with Vector and Value
  EXPECT_EQ(out_config.Find(vect1, "100"), "80");
  EXPECT_EQ(out_config.Find(vect2, "/"), "./");
  EXPECT_EQ(out_config.Find(vect3, "good"), "good");
  EXPECT_EQ(out_config.Find(vect4, "good"), "good");

  EXPECT_NE(out_config.Find(vect4, "good"), "");
}

TEST_F(NginxConfigParserTest, FindBlocks) {
  parser.Parse("example_config", &out_config);

  // FindBlocks with String
  EXPECT_EQ(out_config.FindBlocks("handler").size(), 5);
  EXPECT_NE(out_config.FindBlocks("handler").size(), 0);

  // FindBlocks with Vector
  std::vector<std::string> vect1 = {"handler"};
  EXPECT_EQ(out_config.FindBlocks(vect1).size(), 5);
  EXPECT_NE(out_config.FindBlocks(vect1).size(), 0);

  // // FindBlocks with Vector and Value
  std::vector<NginxConfig*> blocks1;
  EXPECT_EQ(out_config.FindBlocks(vect1, blocks1).size(), 5);
  EXPECT_NE(out_config.FindBlocks(vect1, blocks1).size(), 0);

  std::vector<std::string> locations = {"/static", "/static2", "/echo", "/", "/status"};
  std::vector<NginxConfig*> configs = out_config.FindBlocks("handler");
  for (int i = 0; i < locations.size(); ++i) {
    EXPECT_EQ(configs[i]->Find("location"), locations[i]);
    EXPECT_NE(configs[i]->Find("location"), "");
  }
}

TEST_F(NginxConfigParserTest, InvalidConfigStreamMissingSemicolon) {
  std::stringstream config_file_stream("key value");

  EXPECT_THROW(parser.Parse(&config_file_stream, &out_config), ConfigParserException);
}

TEST_F(NginxConfigParserTest, InvalidConfigStreamSemicolon) {
  std::stringstream config_file_stream("server {};");

  EXPECT_THROW(parser.Parse(&config_file_stream, &out_config), ConfigParserException);
}

TEST_F(NginxConfigParserTest, InvalidConfigStreamSingleQuote) {
  std::stringstream config_file_stream("key 'value;");

  EXPECT_THROW(parser.Parse(&config_file_stream, &out_config), ConfigParserException);
}

TEST_F(NginxConfigParserTest, InvalidConfigStreamDoubleQuote) {
  std::stringstream config_file_stream("key \"value;");

  EXPECT_THROW(parser.Parse(&config_file_stream, &out_config), ConfigParserException);
}

TEST_F(NginxConfigParserTest, InvalidConfigStreamBlocks) {
  std::stringstream config_file_stream("server {");
  EXPECT_THROW(parser.Parse(&config_file_stream, &out_config), ConfigParserException);

  config_file_stream.clear();
  config_file_stream.str("server {{}}");
  
  EXPECT_THROW(parser.Parse(&config_file_stream, &out_config), ConfigParserException);
  
  config_file_stream.clear();
  config_file_stream.str("server }");

  EXPECT_THROW(parser.Parse(&config_file_stream, &out_config), ConfigParserException);

  config_file_stream.clear();
  config_file_stream.str("\
http {\n\
  server {\n\
    listen   80;\n\
}");

  EXPECT_THROW(parser.Parse(&config_file_stream, &out_config), ConfigParserException);
}

TEST_F(NginxConfigParserTest, ValidConfigStreamEmpty) {
  std::stringstream config_file_stream("");
  success = parser.Parse(&config_file_stream, &out_config);
  
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, ValidConfigStreamComment) {
  std::stringstream config_file_stream(
"key value;\n\
#comment\n");
  success = parser.Parse(&config_file_stream, &out_config);
  
  EXPECT_TRUE(success);
}

TEST(NginxConfigStatementTest, ToString) {
  NginxConfigStatement statement;
  statement.tokens_.push_back("key");
  statement.tokens_.push_back("value");

  EXPECT_EQ(statement.ToString(0), "key value;\n");
  EXPECT_EQ(statement.ToString(1), "  key value;\n");
}

TEST(NginxConfigStatementTest, Find) {
  NginxConfigStatement statement;
  statement.tokens_.push_back("key");
  statement.tokens_.push_back("value");

  std::vector<std::string> vect1 = {"key"};
  std::vector<std::string> vect2 = {"server", "listen"};

  EXPECT_EQ(statement.Find(vect1, ""), "value");
  EXPECT_EQ(statement.Find(vect1, "val"), "value");
  EXPECT_EQ(statement.Find(vect2, ""), "");
  EXPECT_EQ(statement.Find(vect2, "val"), "val");

  EXPECT_NE(statement.Find(vect1, "val"), "");
  EXPECT_NE(statement.Find(vect2, "val"), "");
}