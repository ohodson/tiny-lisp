#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace lisp {

enum class TokenType {
    LPAREN,     // (
    RPAREN,     // )
    NUMBER,     // 123, 3.14
    STRING,     // "hello"
    SYMBOL,     // +, car, define
    QUOTE,      // '
    EOF_TOKEN
};

struct Token {
    TokenType type;
    std::string value;
    size_t position;
    
    Token(TokenType t, const std::string& v, size_t pos) 
        : type(t), value(v), position(pos) {}
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
    Tokenizer(const std::string& input);
    std::vector<Token> tokenize();
    Token next_token();
};

}