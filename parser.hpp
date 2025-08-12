#pragma once

#include <stdexcept>

#include "tokenizer.hpp"
#include "value.hpp"

namespace lisp {

class ParseError : public std::runtime_error {
 public:
  ParseError(const std::string& message) : std::runtime_error(message) {}
};

class Parser {
 private:
  std::vector<Token> tokens;
  size_t position;

  const Token& current_token() const;
  const Token& peek_token(size_t offset = 1) const;
  void advance();
  bool is_at_end() const;

  ValuePtr parse_atom();
  ValuePtr parse_list();
  ValuePtr parse_quoted();

 public:
  Parser(const std::vector<Token>& tokens);
  ValuePtr parse();
  std::vector<ValuePtr> parse_multiple();
};

}  // namespace lisp