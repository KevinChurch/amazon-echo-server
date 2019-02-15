// An nginx config file parser.
//
// See:
//   http://wiki.nginx.org/Configuration
//   http://blog.martinfjordvald.com/2010/07/nginx-primer/
//
// How Nginx does it:
//   http://lxr.nginx.org/source/src/core/ngx_conf_file.c

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <vector>

#include "config_parser.h"
#include "config_parser_exception.h"

std::string NginxConfig::ToString(int depth) const {
  std::string serialized_config;
  for (const auto& statement : statements_) {
    serialized_config.append(statement->ToString(depth));
  }
  return serialized_config;
}

std::string NginxConfig::Find(std::string key) const {
  std::vector<std::string> vectorKey;
  boost::split(vectorKey, key, boost::is_any_of("."));
  return Find(vectorKey);
}

std::string NginxConfig::Find(std::vector<std::string> vectorKey) const {
  std::string value;
  return Find(vectorKey, value);
}

std::string NginxConfig::Find(std::vector<std::string> vectorKey, std::string value) const {
  for (const auto& statement : statements_) {
    value = statement->Find(vectorKey, value);
  }
  return value;
}

std::string NginxConfigStatement::Find(std::vector<std::string> vectorKey, std::string value) const {
  std::string key = vectorKey.front();

  if (!tokens_.empty() && (tokens_[0] == key)) {
    if (vectorKey.size() == 1) {
      value = tokens_[1];
    } else if (child_block_.get() != nullptr) {
      std::vector<std::string> newVec(vectorKey.begin()+1, vectorKey.end());
      value = child_block_->Find(newVec, value);
    }
  }
  
  return value;
}

std::vector<NginxConfig*> NginxConfig::FindBlocks(std::string key) const {
  std::vector<std::string> vectorKey;
  boost::split(vectorKey, key, boost::is_any_of("."));
  return FindBlocks(vectorKey);
}

std::vector<NginxConfig*> NginxConfig::FindBlocks(std::vector<std::string> vectorKey) const {
  std::vector<NginxConfig*> blocks;
  return FindBlocks(vectorKey, blocks);
}

std::vector<NginxConfig*> NginxConfig::FindBlocks(std::vector<std::string> vectorKey, std::vector<NginxConfig*> blocks) const {
  for (const auto& statement : statements_) {
    blocks = statement->FindBlocks(vectorKey, blocks);
  }
  return blocks;
}

std::vector<NginxConfig*> NginxConfigStatement::FindBlocks(std::vector<std::string> vectorKey, std::vector<NginxConfig*> blocks) const {
  std::string key = vectorKey.front();

  if (!tokens_.empty()) {
    if (tokens_[0] == key) {
      if (vectorKey.size() == 1) {
        NginxConfig *block = child_block_.get();
        if (block != nullptr) {
          blocks.push_back(block);
        }
      } else {
        std::vector<std::string> newVec(vectorKey.begin()+1, vectorKey.end());
        blocks = FindBlocks(newVec, blocks);
      }
    } else if (child_block_.get() != nullptr) {
      blocks = child_block_->FindBlocks(vectorKey, blocks);
    }
  }
  return blocks;
}

std::string NginxConfigStatement::ToString(int depth) const {
  std::string serialized_statement;
  for (int i = 0; i < depth; ++i) {
    serialized_statement.append("  ");
  }
  for (unsigned int i = 0; i < tokens_.size(); ++i) {
    if (i != 0) {
      serialized_statement.append(" ");
    }
    serialized_statement.append(tokens_[i]);
  }
  if (child_block_.get() != nullptr) {
    serialized_statement.append(" {\n");
    serialized_statement.append(child_block_->ToString(depth + 1));
    for (int i = 0; i < depth; ++i) {
      serialized_statement.append("  ");
    }
    serialized_statement.append("}");
  } else {
    serialized_statement.append(";");
  }
  serialized_statement.append("\n");
  return serialized_statement;
}

const char* NginxConfigParser::TokenTypeAsString(TokenType type) {
  switch (type) {
    case TOKEN_TYPE_START:         return "TOKEN_TYPE_START";
    case TOKEN_TYPE_NORMAL:        return "TOKEN_TYPE_NORMAL";
    case TOKEN_TYPE_START_BLOCK:   return "TOKEN_TYPE_START_BLOCK";
    case TOKEN_TYPE_END_BLOCK:     return "TOKEN_TYPE_END_BLOCK";
    case TOKEN_TYPE_COMMENT:       return "TOKEN_TYPE_COMMENT";
    case TOKEN_TYPE_STATEMENT_END: return "TOKEN_TYPE_STATEMENT_END";
    case TOKEN_TYPE_EOF:           return "TOKEN_TYPE_EOF";
    case TOKEN_TYPE_ERROR:         return "TOKEN_TYPE_ERROR";
    default:                       return "Unknown token type";
  }
}

NginxConfigParser::TokenType NginxConfigParser::ParseToken(std::istream* input,
                                                           std::string* value) {
  TokenParserState state = TOKEN_STATE_INITIAL_WHITESPACE;
  while (input->good()) {
    const char c = input->get();
    if (!input->good()) {
      break;
    }
    switch (state) {
      case TOKEN_STATE_INITIAL_WHITESPACE:
        switch (c) {
          case '{':
            *value = c;
            return TOKEN_TYPE_START_BLOCK;
          case '}':
            *value = c;
            return TOKEN_TYPE_END_BLOCK;
          case '#':
            *value = c;
            state = TOKEN_STATE_TOKEN_TYPE_COMMENT;
            continue;
          case '"':
            *value = c;
            state = TOKEN_STATE_DOUBLE_QUOTE;
            continue;
          case '\'':
            *value = c;
            state = TOKEN_STATE_SINGLE_QUOTE;
            continue;
          case ';':
            *value = c;
            return TOKEN_TYPE_STATEMENT_END;
          case ' ':
          case '\t':
          case '\n':
          case '\r':
            continue;
          default:
            *value += c;
            state = TOKEN_STATE_TOKEN_TYPE_NORMAL;
            continue;
        }
      case TOKEN_STATE_SINGLE_QUOTE:
        // TODO: the end of a quoted token should be followed by whitespace.
        // TODO: Maybe also define a QUOTED_STRING token type.
        // TODO: Allow for backslash-escaping within strings.
        *value += c;
        if (c == '\'') {
          return TOKEN_TYPE_NORMAL;
        }
        continue;
      case TOKEN_STATE_DOUBLE_QUOTE:
        *value += c;
        if (c == '"') {
          return TOKEN_TYPE_NORMAL;
        }
        continue;
      case TOKEN_STATE_TOKEN_TYPE_COMMENT:
        if (c == '\n' || c == '\r') {
          return TOKEN_TYPE_COMMENT;
        }
        *value += c;
        continue;
      case TOKEN_STATE_TOKEN_TYPE_NORMAL:
        if (c == ' ' || c == '\t' || c == '\n' || c == '\t' ||
            c == ';' || c == '{' || c == '}') {
          input->unget();
          return TOKEN_TYPE_NORMAL;
        }
        *value += c;
        continue;
    }
  }

  // If we get here, we reached the end of the file.
  if (state == TOKEN_STATE_SINGLE_QUOTE ||
      state == TOKEN_STATE_DOUBLE_QUOTE) {
    return TOKEN_TYPE_ERROR;
  }

  return TOKEN_TYPE_EOF;
}

bool NginxConfigParser::Parse(std::istream* config_file, NginxConfig* config) {
  std::stack<NginxConfig*> config_stack;
  config_stack.push(config);
  TokenType last_token_type = TOKEN_TYPE_START;
  TokenType token_type;
  int block_count = 0;
  while (true) {
    std::string token;
    token_type = ParseToken(config_file, &token);
    // printf ("%s: %s\n", TokenTypeAsString(token_type), token.c_str());
    if (token_type == TOKEN_TYPE_ERROR) {
      break;
    }

    if (token_type == TOKEN_TYPE_COMMENT) {
      // Skip comments.
      continue;
    }

    if (token_type == TOKEN_TYPE_START) {
      // Error.
      break;
    } else if (token_type == TOKEN_TYPE_NORMAL) {
      if (last_token_type == TOKEN_TYPE_START ||
          last_token_type == TOKEN_TYPE_STATEMENT_END ||
          last_token_type == TOKEN_TYPE_START_BLOCK ||
          last_token_type == TOKEN_TYPE_END_BLOCK ||
          last_token_type == TOKEN_TYPE_NORMAL) {
        if (last_token_type != TOKEN_TYPE_NORMAL) {
          config_stack.top()->statements_.emplace_back(
              new NginxConfigStatement);
        }
        config_stack.top()->statements_.back().get()->tokens_.push_back(
            token);
      } else {
        // Error.
        break;
      }
    } else if (token_type == TOKEN_TYPE_STATEMENT_END) {
      if (last_token_type != TOKEN_TYPE_NORMAL) {
        // Error.
        break;
      }
    } else if (token_type == TOKEN_TYPE_START_BLOCK) {
      if (last_token_type != TOKEN_TYPE_NORMAL) {
        // Error.
        break;
      }
      NginxConfig* const new_config = new NginxConfig;
      config_stack.top()->statements_.back().get()->child_block_.reset(
          new_config);
      config_stack.push(new_config);
      ++block_count;
    } else if (token_type == TOKEN_TYPE_END_BLOCK) {
      if ((last_token_type != TOKEN_TYPE_STATEMENT_END &&
          last_token_type != TOKEN_TYPE_END_BLOCK) ||
          !block_count) {
        // Error.
        break;
      }
      config_stack.pop();
      --block_count;
    } else if (token_type == TOKEN_TYPE_EOF) {
      if ((last_token_type != TOKEN_TYPE_STATEMENT_END &&
          last_token_type != TOKEN_TYPE_END_BLOCK && 
          last_token_type != TOKEN_TYPE_START) || 
          block_count) {
        // Error.
        break;
      }
      return true;
    } else {
      // Error. Unknown token.
      break;
    }
    last_token_type = token_type;
  }

  printf ("Bad transition from %s to %s\n",
          TokenTypeAsString(last_token_type),
          TokenTypeAsString(token_type));

  throw ConfigParserException();
  return false;
}

bool NginxConfigParser::Parse(const char* file_name, NginxConfig* config) {
  std::ifstream config_file;
  config_file.open(file_name);
  if (!config_file.good()) {
    printf ("Failed to open config file: %s\n", file_name);
    return false;
  }

  const bool return_value =
      Parse(dynamic_cast<std::istream*>(&config_file), config);
  config_file.close();
  return return_value;
}
