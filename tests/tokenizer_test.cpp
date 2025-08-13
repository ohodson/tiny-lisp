#include "tokenizer.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace lisp {

class TokenizerTest : public ::testing::Test {
 protected:
  static void tokenize_and_check(
      const std::string& input,
      const std::vector<std::pair<TokenType, std::string>>& expected) {
    Tokenizer tokenizer(input);
    auto tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), expected.size() + 1);  // +1 for EOF_TOKEN

    for (size_t i = 0; i < expected.size(); ++i) {
      EXPECT_EQ(tokens[i].type(), expected[i].first)
          << "Token " << i << " type mismatch";
      EXPECT_EQ(tokens[i].value(), expected[i].second)
          << "Token " << i << " value mismatch";
    }

    EXPECT_EQ(tokens.back().type(), TokenType::EOF_TOKEN);
  }
};

TEST_F(TokenizerTest, EmptyInput) {
  Tokenizer tokenizer("");
  auto tokens = tokenizer.tokenize();
  ASSERT_EQ(tokens.size(), 1);
  EXPECT_EQ(tokens[0].type(), TokenType::EOF_TOKEN);
}

TEST_F(TokenizerTest, WhitespaceOnly) {
  Tokenizer tokenizer("   \t\n  ");
  auto tokens = tokenizer.tokenize();
  ASSERT_EQ(tokens.size(), 1);
  EXPECT_EQ(tokens[0].type(), TokenType::EOF_TOKEN);
}

TEST_F(TokenizerTest, SingleParentheses) {
  tokenize_and_check("(", {{TokenType::LPAREN, "("}});
  tokenize_and_check(")", {{TokenType::RPAREN, ")"}});
  tokenize_and_check("()",
                     {{TokenType::LPAREN, "("}, {TokenType::RPAREN, ")"}});
}

TEST_F(TokenizerTest, Quote) {
  tokenize_and_check("'", {{TokenType::QUOTE, "'"}});
  tokenize_and_check("'symbol",
                     {{TokenType::QUOTE, "'"}, {TokenType::SYMBOL, "symbol"}});
}

TEST_F(TokenizerTest, Numbers) {
  tokenize_and_check("42", {{TokenType::NUMBER, "42"}});
  tokenize_and_check("3.14", {{TokenType::NUMBER, "3.14"}});
  tokenize_and_check("-42", {{TokenType::NUMBER, "-42"}});
  tokenize_and_check("+42", {{TokenType::NUMBER, "+42"}});
  tokenize_and_check("-3.14", {{TokenType::NUMBER, "-3.14"}});
  tokenize_and_check("+3.14", {{TokenType::NUMBER, "+3.14"}});
  tokenize_and_check("0", {{TokenType::NUMBER, "0"}});
  tokenize_and_check("0.0", {{TokenType::NUMBER, "0.0"}});
}

TEST_F(TokenizerTest, Symbols) {
  tokenize_and_check("hello", {{TokenType::SYMBOL, "hello"}});
  tokenize_and_check("foo-bar", {{TokenType::SYMBOL, "foo-bar"}});
  tokenize_and_check("+", {{TokenType::SYMBOL, "+"}});
  tokenize_and_check("-", {{TokenType::SYMBOL, "-"}});
  tokenize_and_check("*", {{TokenType::SYMBOL, "*"}});
  tokenize_and_check("/", {{TokenType::SYMBOL, "/"}});
  tokenize_and_check("=", {{TokenType::SYMBOL, "="}});
  tokenize_and_check("car", {{TokenType::SYMBOL, "car"}});
  tokenize_and_check("cdr", {{TokenType::SYMBOL, "cdr"}});
  tokenize_and_check("define", {{TokenType::SYMBOL, "define"}});
  tokenize_and_check("lambda", {{TokenType::SYMBOL, "lambda"}});
}

TEST_F(TokenizerTest, Strings) {
  tokenize_and_check("\"hello\"", {{TokenType::STRING, "hello"}});
  tokenize_and_check("\"hello world\"", {{TokenType::STRING, "hello world"}});
  tokenize_and_check("\"\"", {{TokenType::STRING, ""}});
  tokenize_and_check("\"foo bar baz\"", {{TokenType::STRING, "foo bar baz"}});
}

TEST_F(TokenizerTest, StringEscapes) {
  tokenize_and_check("\"\\n\"", {{TokenType::STRING, "\n"}});
  tokenize_and_check("\"\\t\"", {{TokenType::STRING, "\t"}});
  tokenize_and_check("\"\\r\"", {{TokenType::STRING, "\r"}});
  tokenize_and_check("\"\\\\\"", {{TokenType::STRING, "\\"}});
  tokenize_and_check("\"\\\"\"", {{TokenType::STRING, "\""}});
  tokenize_and_check("\"hello\\nworld\"",
                     {{TokenType::STRING, "hello\nworld"}});
  tokenize_and_check("\"tab\\there\"", {{TokenType::STRING, "tab\there"}});
}

TEST_F(TokenizerTest, Comments) {
  tokenize_and_check("; comment", {});
  tokenize_and_check("42 ; comment", {{TokenType::NUMBER, "42"}});
  tokenize_and_check("; comment\n42", {{TokenType::NUMBER, "42"}});
  tokenize_and_check("(+ 1 2) ; add numbers", {{TokenType::LPAREN, "("},
                                               {TokenType::SYMBOL, "+"},
                                               {TokenType::NUMBER, "1"},
                                               {TokenType::NUMBER, "2"},
                                               {TokenType::RPAREN, ")"}});
}

TEST_F(TokenizerTest, CommentsAndWhitespaceCombined) {
  const char* kInput = R"(
    ; This is a comment.
    ; This is another comment.

    ; This is a comment after a blank line.
    (define x 10) ; define x
    (define y 20) ; define y

    ; Another comment

    (+ x y) ; add x and y
  )";
  tokenize_and_check(kInput, {{TokenType::LPAREN, "("},
                               {TokenType::SYMBOL, "define"},
                               {TokenType::SYMBOL, "x"},
                               {TokenType::NUMBER, "10"},
                               {TokenType::RPAREN, ")"},
                               {TokenType::LPAREN, "("},
                               {TokenType::SYMBOL, "define"},
                               {TokenType::SYMBOL, "y"},
                               {TokenType::NUMBER, "20"},
                               {TokenType::RPAREN, ")"},
                               {TokenType::LPAREN, "("},
                               {TokenType::SYMBOL, "+"},
                               {TokenType::SYMBOL, "x"},
                               {TokenType::SYMBOL, "y"},
                               {TokenType::RPAREN, ")"}});
}

TEST_F(TokenizerTest, ComplexExpressions) {
  tokenize_and_check("(+ 1 2)", {{TokenType::LPAREN, "("},
                                 {TokenType::SYMBOL, "+"},
                                 {TokenType::NUMBER, "1"},
                                 {TokenType::NUMBER, "2"},
                                 {TokenType::RPAREN, ")"}});

  tokenize_and_check("(define x 42)", {{TokenType::LPAREN, "("},
                                       {TokenType::SYMBOL, "define"},
                                       {TokenType::SYMBOL, "x"},
                                       {TokenType::NUMBER, "42"},
                                       {TokenType::RPAREN, ")"}});

  tokenize_and_check("(lambda (x y) (+ x y))", {{TokenType::LPAREN, "("},
                                                {TokenType::SYMBOL, "lambda"},
                                                {TokenType::LPAREN, "("},
                                                {TokenType::SYMBOL, "x"},
                                                {TokenType::SYMBOL, "y"},
                                                {TokenType::RPAREN, ")"},
                                                {TokenType::LPAREN, "("},
                                                {TokenType::SYMBOL, "+"},
                                                {TokenType::SYMBOL, "x"},
                                                {TokenType::SYMBOL, "y"},
                                                {TokenType::RPAREN, ")"},
                                                {TokenType::RPAREN, ")"}});
}

TEST_F(TokenizerTest, QuotedExpressions) {
  tokenize_and_check("'(1 2 3)", {{TokenType::QUOTE, "'"},
                                  {TokenType::LPAREN, "("},
                                  {TokenType::NUMBER, "1"},
                                  {TokenType::NUMBER, "2"},
                                  {TokenType::NUMBER, "3"},
                                  {TokenType::RPAREN, ")"}});

  tokenize_and_check("'symbol",
                     {{TokenType::QUOTE, "'"}, {TokenType::SYMBOL, "symbol"}});
}

TEST_F(TokenizerTest, MixedTypes) {
  tokenize_and_check("(car '(\"hello\" 42 world))",
                     {{TokenType::LPAREN, "("},
                      {TokenType::SYMBOL, "car"},
                      {TokenType::QUOTE, "'"},
                      {TokenType::LPAREN, "("},
                      {TokenType::STRING, "hello"},
                      {TokenType::NUMBER, "42"},
                      {TokenType::SYMBOL, "world"},
                      {TokenType::RPAREN, ")"},
                      {TokenType::RPAREN, ")"}});
}

TEST_F(TokenizerTest, MultilineInput) {
  std::string const input = "(define foo (+ 1 2))";

  tokenize_and_check(input, {{TokenType::LPAREN, "("},
                             {TokenType::SYMBOL, "define"},
                             {TokenType::SYMBOL, "foo"},
                             {TokenType::LPAREN, "("},
                             {TokenType::SYMBOL, "+"},
                             {TokenType::NUMBER, "1"},
                             {TokenType::NUMBER, "2"},
                             {TokenType::RPAREN, ")"},
                             {TokenType::RPAREN, ")"}});
}

TEST_F(TokenizerTest, TokenPositions) {
  Tokenizer tokenizer("(+ 1 2)");
  auto tokens = tokenizer.tokenize();

  ASSERT_EQ(tokens.size(), 6);         // (, +, 1, 2, ), EOF
  EXPECT_EQ(tokens[0].position(), 0);  // (
  EXPECT_EQ(tokens[1].position(), 1);  // +
  EXPECT_EQ(tokens[2].position(), 3);  // 1
  EXPECT_EQ(tokens[3].position(), 5);  // 2
  EXPECT_EQ(tokens[4].position(), 6);  // )
}

TEST_F(TokenizerTest, NextTokenMethod) {
  Tokenizer tokenizer("(+ 1)");

  auto token1 = tokenizer.next_token();
  EXPECT_EQ(token1.type(), TokenType::LPAREN);
  EXPECT_EQ(token1.value(), "(");

  auto token2 = tokenizer.next_token();
  EXPECT_EQ(token2.type(), TokenType::SYMBOL);
  EXPECT_EQ(token2.value(), "+");

  auto token3 = tokenizer.next_token();
  EXPECT_EQ(token3.type(), TokenType::NUMBER);
  EXPECT_EQ(token3.value(), "1");

  auto token4 = tokenizer.next_token();
  EXPECT_EQ(token4.type(), TokenType::RPAREN);
  EXPECT_EQ(token4.value(), ")");

  auto token5 = tokenizer.next_token();
  EXPECT_EQ(token5.type(), TokenType::EOF_TOKEN);
}

}  // namespace lisp
