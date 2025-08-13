#include "tokenizer.hpp"

#include <cctype>
#include <cstddef>
#include <string>
#include <vector>

namespace lisp {

Tokenizer::Tokenizer(const std::string& input)
    : input(input), position(0), length(input.length()) {}

char Tokenizer::peek(size_t offset) const {
  if (size_t const pos = position + offset; pos < length) {
    return input[pos];
  }
  return '\0';
}

char Tokenizer::advance() {
  if (position < length) {
    return input[position++];
  }
  return '\0';
}

void Tokenizer::skip_whitespace() {
  while (position < length && std::isspace(peek()) != 0) {
    advance();
  }
}

void Tokenizer::skip_comment() {
  if (peek() == ';') {
    while (position < length && peek() != '\n') {
      advance();
    }
  }
}

Token Tokenizer::read_number() {
  size_t const start_pos = position;
  std::string number;

  if (peek() == '-' || peek() == '+') {
    number += advance();
  }

  while (position < length && ((std::isdigit(peek()) != 0) || peek() == '.')) {
    number += advance();
  }

  return Token(TokenType::NUMBER, number, start_pos);
}

Token Tokenizer::read_string() {
  size_t const start_pos = position;
  std::string str;

  advance();  // skip opening quote

  while (position < length && peek() != '"') {
    char const glyph = advance();
    if (glyph == '\\' && position < length) {
      char const escaped = advance();
      switch (escaped) {
        case 'n':
          str += '\n';
          break;
        case 't':
          str += '\t';
          break;
        case 'r':
          str += '\r';
          break;
        case '\\':
          str += '\\';
          break;
        case '"':
          str += '"';
          break;
        default:
          str += escaped;
          break;
      }
    } else {
      str += glyph;
    }
  }

  if (position < length && peek() == '"') {
    advance();  // skip closing quote
  }

  return Token(TokenType::STRING, str, start_pos);
}

Token Tokenizer::read_symbol() {
  size_t const start_pos = position;
  std::string symbol;

  while (position < length && (std::isspace(peek()) == 0) && peek() != '(' &&
         peek() != ')' && peek() != '"' && peek() != ';') {
    symbol += advance();
  }

  return Token(TokenType::SYMBOL, symbol, start_pos);
}

Token Tokenizer::next_token() {
  skip_whitespace();
  skip_comment();
  skip_whitespace();

  if (position >= length) {
    return Token(TokenType::EOF_TOKEN, "", position);
  }

  char const glyph = peek();
  size_t const start_pos = position;

  switch (glyph) {
    case '(':
      advance();
      return Token(TokenType::LPAREN, "(", start_pos);
    case ')':
      advance();
      return Token(TokenType::RPAREN, ")", start_pos);
    case '\'':
      advance();
      return Token(TokenType::QUOTE, "'", start_pos);
    case '"':
      return read_string();
    default:
      if ((std::isdigit(glyph) != 0) ||
          (glyph == '-' && (std::isdigit(peek(1)) != 0)) ||
          (glyph == '+' && (std::isdigit(peek(1)) != 0))) {
        return read_number();
      } else {
        return read_symbol();
      }
  }
}

std::vector<Token> Tokenizer::tokenize() {
  std::vector<Token> tokens;

  while (true) {
    Token const token = next_token();
    tokens.push_back(token);
    if (token.type() == TokenType::EOF_TOKEN) {
      break;
    }
  }

  return tokens;
}

}  // namespace lisp