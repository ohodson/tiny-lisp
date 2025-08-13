#include "parser.hpp"

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "tokenizer.hpp"
#include "value.hpp"

namespace lisp {

class ParserTest : public ::testing::Test {
 protected:
  static ValuePtr parse_string(const std::string& input) {
    Tokenizer tokenizer(input);
    auto tokens = tokenizer.tokenize();
    Parser parser(tokens);
    return parser.parse();
  }

  static std::vector<ValuePtr> parse_multiple_string(const std::string& input) {
    Tokenizer tokenizer(input);
    auto tokens = tokenizer.tokenize();
    Parser parser(tokens);
    return parser.parse_multiple();
  }
};

TEST_F(ParserTest, ParseNumbers) {
  auto result = parse_string("42");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 42.0);

  result = parse_string("3.14");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 3.14);

  result = parse_string("-17");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), -17.0);

  result = parse_string("+123");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 123.0);
}

TEST_F(ParserTest, ParseStrings) {
  auto result = parse_string("\"hello\"");
  EXPECT_TRUE(result->is_string());
  EXPECT_EQ(result->as_string(), "hello");

  result = parse_string("\"hello world\"");
  EXPECT_TRUE(result->is_string());
  EXPECT_EQ(result->as_string(), "hello world");

  result = parse_string("\"\"");
  EXPECT_TRUE(result->is_string());
  EXPECT_EQ(result->as_string(), "");
}

TEST_F(ParserTest, ParseSymbols) {
  auto result = parse_string("hello");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "hello");

  result = parse_string("+");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "+");

  result = parse_string("define");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "define");

  result = parse_string("foo-bar");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "foo-bar");
}

TEST_F(ParserTest, ParseNil) {
  auto result = parse_string("nil");
  EXPECT_TRUE(result->is_nil());
}

TEST_F(ParserTest, ParseEmptyList) {
  auto result = parse_string("()");
  EXPECT_TRUE(result->is_nil());
}

TEST_F(ParserTest, ParseSimpleList) {
  auto result = parse_string("(1 2 3)");
  EXPECT_TRUE(result->is_cons());

  EXPECT_TRUE(result->car()->is_number());
  EXPECT_DOUBLE_EQ(result->car()->as_number(), 1.0);

  auto cdr1 = result->cdr();
  EXPECT_TRUE(cdr1->is_cons());
  EXPECT_TRUE(cdr1->car()->is_number());
  EXPECT_DOUBLE_EQ(cdr1->car()->as_number(), 2.0);

  auto cdr2 = cdr1->cdr();
  EXPECT_TRUE(cdr2->is_cons());
  EXPECT_TRUE(cdr2->car()->is_number());
  EXPECT_DOUBLE_EQ(cdr2->car()->as_number(), 3.0);

  auto cdr3 = cdr2->cdr();
  EXPECT_TRUE(cdr3->is_nil());
}

TEST_F(ParserTest, ParseNestedList) {
  auto result = parse_string("((1 2) (3 4))");
  EXPECT_TRUE(result->is_cons());

  auto first_elem = result->car();
  EXPECT_TRUE(first_elem->is_cons());
  EXPECT_TRUE(first_elem->car()->is_number());
  EXPECT_DOUBLE_EQ(first_elem->car()->as_number(), 1.0);
  EXPECT_TRUE(first_elem->cdr()->car()->is_number());
  EXPECT_DOUBLE_EQ(first_elem->cdr()->car()->as_number(), 2.0);

  auto second_elem = result->cdr()->car();
  EXPECT_TRUE(second_elem->is_cons());
  EXPECT_TRUE(second_elem->car()->is_number());
  EXPECT_DOUBLE_EQ(second_elem->car()->as_number(), 3.0);
  EXPECT_TRUE(second_elem->cdr()->car()->is_number());
  EXPECT_DOUBLE_EQ(second_elem->cdr()->car()->as_number(), 4.0);
}

TEST_F(ParserTest, ParseMixedTypes) {
  auto result = parse_string("(+ \"hello\" 42 world)");
  EXPECT_TRUE(result->is_cons());

  EXPECT_TRUE(result->car()->is_symbol());
  EXPECT_EQ(result->car()->as_symbol(), "+");

  auto rest = result->cdr();
  EXPECT_TRUE(rest->car()->is_string());
  EXPECT_EQ(rest->car()->as_string(), "hello");

  rest = rest->cdr();
  EXPECT_TRUE(rest->car()->is_number());
  EXPECT_DOUBLE_EQ(rest->car()->as_number(), 42.0);

  rest = rest->cdr();
  EXPECT_TRUE(rest->car()->is_symbol());
  EXPECT_EQ(rest->car()->as_symbol(), "world");

  EXPECT_TRUE(rest->cdr()->is_nil());
}

TEST_F(ParserTest, ParseQuotedAtom) {
  auto result = parse_string("'hello");
  EXPECT_TRUE(result->is_cons());

  EXPECT_TRUE(result->car()->is_symbol());
  EXPECT_EQ(result->car()->as_symbol(), "quote");

  auto quoted = result->cdr();
  EXPECT_TRUE(quoted->is_cons());
  EXPECT_TRUE(quoted->car()->is_symbol());
  EXPECT_EQ(quoted->car()->as_symbol(), "hello");
  EXPECT_TRUE(quoted->cdr()->is_nil());
}

TEST_F(ParserTest, ParseQuotedList) {
  auto result = parse_string("'(1 2 3)");
  EXPECT_TRUE(result->is_cons());

  EXPECT_TRUE(result->car()->is_symbol());
  EXPECT_EQ(result->car()->as_symbol(), "quote");

  auto quoted = result->cdr();
  EXPECT_TRUE(quoted->is_cons());
  auto quoted_list = quoted->car();
  EXPECT_TRUE(quoted_list->is_cons());

  EXPECT_TRUE(quoted_list->car()->is_number());
  EXPECT_DOUBLE_EQ(quoted_list->car()->as_number(), 1.0);
  EXPECT_TRUE(quoted_list->cdr()->car()->is_number());
  EXPECT_DOUBLE_EQ(quoted_list->cdr()->car()->as_number(), 2.0);
  EXPECT_TRUE(quoted_list->cdr()->cdr()->car()->is_number());
  EXPECT_DOUBLE_EQ(quoted_list->cdr()->cdr()->car()->as_number(), 3.0);
}

TEST_F(ParserTest, ParseComplexExpression) {
  auto result = parse_string(
      "(define factorial (lambda (n) (if (= n 0) 1 (* n (factorial (- n "
      "1))))))");
  EXPECT_TRUE(result->is_cons());

  EXPECT_TRUE(result->car()->is_symbol());
  EXPECT_EQ(result->car()->as_symbol(), "define");

  auto rest = result->cdr();
  EXPECT_TRUE(rest->car()->is_symbol());
  EXPECT_EQ(rest->car()->as_symbol(), "factorial");

  rest = rest->cdr();
  auto lambda_expr = rest->car();
  EXPECT_TRUE(lambda_expr->is_cons());
  EXPECT_TRUE(lambda_expr->car()->is_symbol());
  EXPECT_EQ(lambda_expr->car()->as_symbol(), "lambda");
}

TEST_F(ParserTest, ParseMultipleExpressions) {
  auto results = parse_multiple_string("42 \"hello\" (+ 1 2)");
  EXPECT_EQ(results.size(), 3);

  EXPECT_TRUE(results[0]->is_number());
  EXPECT_DOUBLE_EQ(results[0]->as_number(), 42.0);

  EXPECT_TRUE(results[1]->is_string());
  EXPECT_EQ(results[1]->as_string(), "hello");

  EXPECT_TRUE(results[2]->is_cons());
  EXPECT_TRUE(results[2]->car()->is_symbol());
  EXPECT_EQ(results[2]->car()->as_symbol(), "+");
}

TEST_F(ParserTest, ParseEmptyInput) {
  auto results = parse_multiple_string("");
  EXPECT_EQ(results.size(), 0);
}

TEST_F(ParserTest, ParseErrorTests) {
  EXPECT_THROW(parse_string(""), ParseError);
  EXPECT_THROW(parse_string("("), ParseError);
  EXPECT_THROW(parse_string(")"), ParseError);
  EXPECT_THROW(parse_string("(1 2"), ParseError);
  EXPECT_THROW(parse_string("'"), ParseError);

  // Note: "1 2)" will parse "1" successfully and ignore "2)"
  // This is expected behavior for our parser
}

TEST_F(ParserTest, ParseWithComments) {
  auto result = parse_string("42 ; this is a comment");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 42.0);

  result = parse_string("(+ 1 2) ; add numbers");
  EXPECT_TRUE(result->is_cons());
  EXPECT_TRUE(result->car()->is_symbol());
  EXPECT_EQ(result->car()->as_symbol(), "+");
}

TEST_F(ParserTest, ParseNestedQuotes) {
  auto result = parse_string("'(quote x)");
  EXPECT_TRUE(result->is_cons());
  EXPECT_TRUE(result->car()->is_symbol());
  EXPECT_EQ(result->car()->as_symbol(), "quote");

  auto quoted_list = result->cdr()->car();
  EXPECT_TRUE(quoted_list->is_cons());
  EXPECT_TRUE(quoted_list->car()->is_symbol());
  EXPECT_EQ(quoted_list->car()->as_symbol(), "quote");
  EXPECT_TRUE(quoted_list->cdr()->car()->is_symbol());
  EXPECT_EQ(quoted_list->cdr()->car()->as_symbol(), "x");
}

TEST_F(ParserTest, ParseDeeplyNestedList) {
  auto result = parse_string("((((1))))");
  EXPECT_TRUE(result->is_cons());

  auto nested = result;
  for (int i = 0; i < 4; ++i) {
    EXPECT_TRUE(nested->is_cons());
    nested = nested->car();
  }

  EXPECT_TRUE(nested->is_number());
  EXPECT_DOUBLE_EQ(nested->as_number(), 1.0);
}

}  // namespace lisp