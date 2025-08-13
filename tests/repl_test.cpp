#include "repl.hpp"

#include <gtest/gtest.h>

#include <memory>

namespace lisp {

class REPLTest : public ::testing::Test {
 protected:
  void SetUp() override { repl = std::make_unique<REPL>(); }

  // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
  std::unique_ptr<REPL> repl;
};

TEST_F(REPLTest, EvalStringBasicExpressions) {
  auto result = repl->eval_string("42");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 42.0);

  result = repl->eval_string("\"hello world\"");
  EXPECT_TRUE(result->is_string());
  EXPECT_EQ(result->as_string(), "hello world");

  result = repl->eval_string("'symbol");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "symbol");
}

TEST_F(REPLTest, EvalStringArithmetic) {
  auto result = repl->eval_string("(+ 1 2 3)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 6.0);

  result = repl->eval_string("(* 4 5)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 20.0);

  result = repl->eval_string("(- 10 3)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 7.0);

  result = repl->eval_string("(/ 15 3)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 5.0);
}

TEST_F(REPLTest, EvalStringListOperations) {
  auto result = repl->eval_string("(car '(1 2 3))");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 1.0);

  result = repl->eval_string("(cdr '(1 2 3))");
  EXPECT_TRUE(result->is_cons());
  EXPECT_TRUE(result->car()->is_number());
  EXPECT_DOUBLE_EQ(result->car()->as_number(), 2.0);

  result = repl->eval_string("(cons 'a '(b c))");
  EXPECT_TRUE(result->is_cons());
  EXPECT_TRUE(result->car()->is_symbol());
  EXPECT_EQ(result->car()->as_symbol(), "a");

  result = repl->eval_string("(list 1 2 3)");
  EXPECT_TRUE(result->is_cons());
  EXPECT_TRUE(result->car()->is_number());
  EXPECT_DOUBLE_EQ(result->car()->as_number(), 1.0);
}

TEST_F(REPLTest, EvalStringDefineAndLookup) {
  auto result = repl->eval_string("(define x 42)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 42.0);

  result = repl->eval_string("x");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 42.0);

  result = repl->eval_string("(define msg \"hello\")");
  EXPECT_TRUE(result->is_string());
  EXPECT_EQ(result->as_string(), "hello");

  result = repl->eval_string("msg");
  EXPECT_TRUE(result->is_string());
  EXPECT_EQ(result->as_string(), "hello");
}

TEST_F(REPLTest, EvalStringLambdaDefinitionAndCall) {
  auto result = repl->eval_string("(define square (lambda (x) (* x x)))");
  EXPECT_TRUE(result->is_lambda());

  result = repl->eval_string("(square 5)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 25.0);

  result = repl->eval_string("((lambda (x y) (+ x y)) 3 4)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 7.0);
}

TEST_F(REPLTest, EvalStringConditionals) {
  auto result = repl->eval_string("(if (> 5 3) \"yes\" \"no\")");
  EXPECT_TRUE(result->is_string());
  EXPECT_EQ(result->as_string(), "yes");

  result = repl->eval_string("(if (< 5 3) \"yes\" \"no\")");
  EXPECT_TRUE(result->is_string());
  EXPECT_EQ(result->as_string(), "no");

  result = repl->eval_string("(if #t 42)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 42.0);

  result = repl->eval_string("(if nil 42)");
  EXPECT_TRUE(result->is_nil());
}

TEST_F(REPLTest, EvalStringTypePredicates) {
  auto result = repl->eval_string("(number? 42)");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "#t");

  result = repl->eval_string("(string? \"hello\")");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "#t");

  result = repl->eval_string("(symbol? 'foo)");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "#t");

  result = repl->eval_string("(null? nil)");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "#t");

  result = repl->eval_string("(cons? '(1 2))");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "#t");
}

TEST_F(REPLTest, EvalStringMultipleExpressions) {
  auto result = repl->eval_string("(define x 10) (define y 20) (+ x y)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 30.0);

  result = repl->eval_string("x");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 10.0);

  result = repl->eval_string("y");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 20.0);
}

TEST_F(REPLTest, EvalStringComplexExpressions) {
  auto result = repl->eval_string("(+ (* 2 3) (/ 12 4))");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 9.0);

  result = repl->eval_string("(car (cdr '(a b c d)))");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "b");

  repl->eval_string("(define make-adder (lambda (n) (lambda (x) (+ x n))))");
  repl->eval_string("(define add5 (make-adder 5))");
  result = repl->eval_string("(add5 10)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 15.0);
}

TEST_F(REPLTest, EvalStringFactorialRecursion) {
  std::string const factorial_def = R"(
        (define factorial
          (lambda (n)
            (if (= n 0)
                1
                (* n (factorial (- n 1))))))
    )";

  auto result = repl->eval_string(factorial_def);
  EXPECT_TRUE(result->is_lambda());

  result = repl->eval_string("(factorial 5)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 120.0);

  result = repl->eval_string("(factorial 0)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 1.0);
}

TEST_F(REPLTest, EvalStringListProcessing) {
  repl->eval_string(
      "(define length (lambda (lst) (if (null? lst) 0 (+ 1 (length (cdr "
      "lst))))))");

  auto result = repl->eval_string("(length '())");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 0.0);

  result = repl->eval_string("(length '(a b c d))");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 4.0);

  repl->eval_string(R"(
        (define reverse-helper 
          (lambda (lst acc)
            (if (null? lst)
                acc
                (reverse-helper (cdr lst) (cons (car lst) acc)))))
    )");

  repl->eval_string("(define reverse (lambda (lst) (reverse-helper lst '())))");

  result = repl->eval_string("(reverse '(1 2 3))");
  EXPECT_TRUE(result->is_cons());
  EXPECT_TRUE(result->car()->is_number());
  EXPECT_DOUBLE_EQ(result->car()->as_number(), 3.0);
  EXPECT_TRUE(result->cdr()->car()->is_number());
  EXPECT_DOUBLE_EQ(result->cdr()->car()->as_number(), 2.0);
  EXPECT_TRUE(result->cdr()->cdr()->car()->is_number());
  EXPECT_DOUBLE_EQ(result->cdr()->cdr()->car()->as_number(), 1.0);
}

TEST_F(REPLTest, EvalStringEmptyInput) {
  auto result = repl->eval_string("");
  EXPECT_EQ(result, nullptr);

  result = repl->eval_string("   ");
  EXPECT_EQ(result, nullptr);
}

TEST_F(REPLTest, EvalStringComments) {
  auto result = repl->eval_string("42 ; this is a comment");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 42.0);

  result = repl->eval_string("; just a comment");
  EXPECT_EQ(result, nullptr);
}

TEST_F(REPLTest, EvalStringNestedQuotes) {
  auto result = repl->eval_string("'(quote hello)");
  EXPECT_TRUE(result->is_cons());
  EXPECT_TRUE(result->car()->is_symbol());
  EXPECT_EQ(result->car()->as_symbol(), "quote");

  result = repl->eval_string("''hello");
  EXPECT_TRUE(result->is_cons());
  EXPECT_TRUE(result->car()->is_symbol());
  EXPECT_EQ(result->car()->as_symbol(), "quote");
}

TEST_F(REPLTest, PersistentEnvironment) {
  // Test that definitions persist across multiple evaluations
  repl->eval_string("(define x 42)");
  repl->eval_string("(define y 100)");

  auto result = repl->eval_string("(+ x y)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 142.0);

  // Test that functions also persist
  repl->eval_string("(define add-ten (lambda (n) (+ n 10)))");
  result = repl->eval_string("(add-ten 5)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 15.0);

  // Variables should still be accessible
  result = repl->eval_string("x");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 42.0);
}

TEST_F(REPLTest, EvalStringHigherOrderFunctions) {
  repl->eval_string(R"(
        (define map
          (lambda (f lst)
            (if (null? lst)
                '()
                (cons (f (car lst)) (map f (cdr lst))))))
    )");

  repl->eval_string("(define double (lambda (x) (* x 2)))");

  auto result = repl->eval_string("(map double '(1 2 3))");
  EXPECT_TRUE(result->is_cons());
  EXPECT_TRUE(result->car()->is_number());
  EXPECT_DOUBLE_EQ(result->car()->as_number(), 2.0);
  EXPECT_TRUE(result->cdr()->car()->is_number());
  EXPECT_DOUBLE_EQ(result->cdr()->car()->as_number(), 4.0);
  EXPECT_TRUE(result->cdr()->cdr()->car()->is_number());
  EXPECT_DOUBLE_EQ(result->cdr()->cdr()->car()->as_number(), 6.0);
}

}  // namespace lisp