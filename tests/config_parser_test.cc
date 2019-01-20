#include "gtest/gtest.h"
#include "config_parser.h"

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

TEST_F(NginxConfigParserTest, FullConfigFile) {
  success = parser.Parse("example_full_config", &out_config);

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
foo \"bar\";\n\
server {\n\
  listen 80;\n\
  server_name foo.com;\n\
  root /home/ubuntu/sites/foo/;\n\
}\n");
  EXPECT_EQ(out_config.ToString(1), "\
  foo \"bar\";\n\
  server {\n\
    listen 80;\n\
    server_name foo.com;\n\
    root /home/ubuntu/sites/foo/;\n\
  }\n");
}

TEST_F(NginxConfigParserTest, InvalidConfigStreamMissingSemicolon) {
  std::stringstream config_file_stream("key value");
  success = parser.Parse(&config_file_stream, &out_config);

  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, InvalidConfigStreamSemicolon) {
  std::stringstream config_file_stream("server {};");
  success = parser.Parse(&config_file_stream, &out_config);

  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, InvalidConfigStreamSingleQuote) {
  std::stringstream config_file_stream("key 'value;");
  success = parser.Parse(&config_file_stream, &out_config);

  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, InvalidConfigStreamDoubleQuote) {
  std::stringstream config_file_stream("key \"value;");
  success = parser.Parse(&config_file_stream, &out_config);

  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, InvalidConfigStreamBlocks) {
  std::stringstream config_file_stream("server {");
  success = parser.Parse(&config_file_stream, &out_config);
  
  EXPECT_FALSE(success);

  config_file_stream.clear();
  config_file_stream.str("server {{}}");
  success = parser.Parse(&config_file_stream, &out_config);
  
  EXPECT_FALSE(success);
  
  config_file_stream.clear();
  config_file_stream.str("server }");
  success = parser.Parse(&config_file_stream, &out_config);

  EXPECT_FALSE(success);

  config_file_stream.clear();
  config_file_stream.str("\
http {\n\
  server {\n\
    listen   80;\n\
}");
  success = parser.Parse(&config_file_stream, &out_config);

  EXPECT_FALSE(success);
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