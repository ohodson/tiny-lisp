#include "parser.hpp"

#include <vector>

namespace lisp {

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), position(0) {}

const Token& Parser::current_token() const {
    if (position >= tokens.size()) {
        static Token eof_token(TokenType::EOF_TOKEN, "", 0);
        return eof_token;
    }
    return tokens[position];
}

const Token& Parser::peek_token(size_t offset) const {
    size_t peek_pos = position + offset;
    if (peek_pos >= tokens.size()) {
        static Token eof_token(TokenType::EOF_TOKEN, "", 0);
        return eof_token;
    }
    return tokens[peek_pos];
}

void Parser::advance() {
    if (position < tokens.size()) {
        position++;
    }
}

bool Parser::is_at_end() const {
    return current_token().type == TokenType::EOF_TOKEN;
}

ValuePtr Parser::parse_atom() {
    const Token& token = current_token();
    advance();
    
    switch (token.type) {
        case TokenType::NUMBER: {
            double value = std::stod(token.value);
            return make_number(value);
        }
        case TokenType::STRING: {
            return make_string(token.value);
        }
        case TokenType::SYMBOL: {
            if (token.value == "nil") {
                return make_nil();
            }
            return make_symbol(token.value);
        }
        default:
            throw ParseError("Unexpected token: " + token.value);
    }
}

ValuePtr Parser::parse_list() {
    if (current_token().type != TokenType::LPAREN) {
        throw ParseError("Expected '(' at beginning of list");
    }
    advance(); // consume '('
    
    if (current_token().type == TokenType::RPAREN) {
        advance(); // consume ')'
        return make_nil(); // empty list
    }
    
    ValuePtr head = parse();
    ValuePtr tail = make_nil();
    ValuePtr current = make_cons(head, tail);
    ValuePtr result = current;
    
    while (current_token().type != TokenType::RPAREN && !is_at_end()) {
        ValuePtr next_element = parse();
        ValuePtr new_tail = make_cons(next_element, make_nil());
        
        // Update the cdr of the current cons cell
        auto& cons_pair = std::get<std::pair<ValuePtr, ValuePtr>>(current->data);
        cons_pair.second = new_tail;
        current = new_tail;
    }
    
    if (current_token().type != TokenType::RPAREN) {
        throw ParseError("Expected ')' at end of list");
    }
    advance(); // consume ')'
    
    return result;
}

ValuePtr Parser::parse_quoted() {
    if (current_token().type != TokenType::QUOTE) {
        throw ParseError("Expected quote");
    }
    advance(); // consume quote
    
    ValuePtr quoted_expr = parse();
    ValuePtr quote_symbol = make_symbol("quote");
    ValuePtr quoted_list = make_cons(quoted_expr, make_nil());
    
    return make_cons(quote_symbol, quoted_list);
}

ValuePtr Parser::parse() {
    if (is_at_end()) {
        throw ParseError("Unexpected end of input");
    }
    
    const Token& token = current_token();
    
    switch (token.type) {
        case TokenType::LPAREN:
            return parse_list();
        case TokenType::QUOTE:
            return parse_quoted();
        case TokenType::NUMBER:
        case TokenType::STRING:
        case TokenType::SYMBOL:
            return parse_atom();
        default:
            throw ParseError("Unexpected token: " + token.value);
    }
}

std::vector<ValuePtr> Parser::parse_multiple() {
    std::vector<ValuePtr> results;
    
    while (!is_at_end()) {
        if (current_token().type == TokenType::EOF_TOKEN) {
            break;
        }
        results.push_back(parse());
    }
    
    return results;
}

}  // namespace lisp