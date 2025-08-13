#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace lisp {

enum class TokenType : std::uint8_t {
  LPAREN,  // (
  RPAREN,  // )
  NUMBER,  // 123, 3.14
  STRING,  // "hello"
  SYMBOL,  // +, car, define
  QUOTE,   // '
  EOF_TOKEN
};

struct Token {
 private:
  TokenType type_;
  std::string value_;
  size_t position_;

 public:
  Token(TokenType type, const std::string& value, size_t position)
      : type_(type), value_(value), position_(position) {}

  TokenType type() const { return type_; }
  const std::string& value() const { return value_; }
  size_t position() const { return position_; }
};

class Tokenizer {
 private:
  std::string input;
  size_t position;
  size_t length;

  char peek(size_t offset = 0) const;
  char advance();
  void skip_whitespace();
  void skip_comment();
  Token read_number();
  Token read_string();
  Token read_symbol();

 public:
  explicit Tokenizer(const std::string& input);
  std::vector<Token> tokenize();
  Token next_token();
};

}  // namespace lisp