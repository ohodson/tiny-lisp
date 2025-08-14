#include "evaluator.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "parser.hpp"
#include "tokenizer.hpp"
#include "value.hpp"

namespace lisp {

class EvaluatorTest : public ::testing::Test {
 protected:
  void SetUp() override { evaluator = std::make_unique<Evaluator>(); }

  ValuePtr eval_string(std::string const& input) {
    Tokenizer tokenizer(input);
    auto tokens = tokenizer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    return evaluator->eval(ast);
  }

 private:
  std::unique_ptr<Evaluator> evaluator;
};

TEST_F(EvaluatorTest, SelfEvaluatingExpressions) {
  auto result = eval_string("42");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 42.0);

  result = eval_string("3.14");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 3.14);

  result = eval_string("\"hello\"");
  EXPECT_TRUE(result->is_string());
  EXPECT_EQ(result->as_string(), "hello");

  result = eval_string("nil");
  EXPECT_TRUE(result->is_nil());
}

TEST_F(EvaluatorTest, ArithmeticOperations) {
  auto result = eval_string("(+ 1 2 3)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 6.0);

  result = eval_string("(- 10 3 2)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 5.0);

  result = eval_string("(* 2 3 4)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 24.0);

  result = eval_string("(/ 24 2 3)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 4.0);
}

TEST_F(EvaluatorTest, ArithmeticEdgeCases) {
  auto result = eval_string("(+)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 0.0);

  result = eval_string("(+ 42)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 42.0);

  EXPECT_THROW(eval_string("(-)"), EvalError);
  EXPECT_THROW(eval_string("(*)"), EvalError);
  EXPECT_THROW(eval_string("(/)"), EvalError);
  EXPECT_THROW(eval_string("(/ 1 0)"), EvalError);
}

TEST_F(EvaluatorTest, ListOperations) {
  auto result = eval_string("(car '(1 2 3))");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 1.0);

  result = eval_string("(cdr '(1 2 3))");
  EXPECT_TRUE(result->is_cons());
  EXPECT_TRUE(result->car()->is_number());
  EXPECT_DOUBLE_EQ(result->car()->as_number(), 2.0);

  result = eval_string("(cons 1 '(2 3))");
  EXPECT_TRUE(result->is_cons());
  EXPECT_TRUE(result->car()->is_number());
  EXPECT_DOUBLE_EQ(result->car()->as_number(), 1.0);

  result = eval_string("(list 1 2 3)");
  EXPECT_TRUE(result->is_cons());
  EXPECT_TRUE(result->car()->is_number());
  EXPECT_DOUBLE_EQ(result->car()->as_number(), 1.0);
  EXPECT_TRUE(result->cdr()->car()->is_number());
  EXPECT_DOUBLE_EQ(result->cdr()->car()->as_number(), 2.0);
}

TEST_F(EvaluatorTest, ListOperationsOnNil) {
  auto result = eval_string("(car nil)");
  EXPECT_TRUE(result->is_nil());

  result = eval_string("(cdr nil)");
  EXPECT_TRUE(result->is_nil());

  result = eval_string("(car '())");
  EXPECT_TRUE(result->is_nil());

  result = eval_string("(cdr '())");
  EXPECT_TRUE(result->is_nil());
}

TEST_F(EvaluatorTest, ComparisonOperations) {
  auto result = eval_string("(= 1 1)");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "#t");

  result = eval_string("(= 1 2)");
  EXPECT_TRUE(result->is_nil());

  result = eval_string("(< 1 2)");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "#t");

  result = eval_string("(< 2 1)");
  EXPECT_TRUE(result->is_nil());

  result = eval_string("(> 2 1)");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "#t");

  result = eval_string("(> 1 2)");
  EXPECT_TRUE(result->is_nil());
}

TEST_F(EvaluatorTest, StringAndSymbolComparisons) {
  auto result = eval_string("(= \"hello\" \"hello\")");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "#t");

  result = eval_string("(= \"hello\" \"world\")");
  EXPECT_TRUE(result->is_nil());

  result = eval_string("(= 'foo 'foo)");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "#t");

  result = eval_string("(= 'foo 'bar)");
  EXPECT_TRUE(result->is_nil());

  result = eval_string("(= nil nil)");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "#t");
}

TEST_F(EvaluatorTest, TypePredicates) {
  auto result = eval_string("(null? nil)");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "#t");

  result = eval_string("(null? 42)");
  EXPECT_TRUE(result->is_nil());

  result = eval_string("(number? 42)");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "#t");

  result = eval_string("(number? \"hello\")");
  EXPECT_TRUE(result->is_nil());

  result = eval_string("(string? \"hello\")");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "#t");

  result = eval_string("(string? 42)");
  EXPECT_TRUE(result->is_nil());

  result = eval_string("(symbol? 'foo)");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "#t");

  result = eval_string("(symbol? 42)");
  EXPECT_TRUE(result->is_nil());

  result = eval_string("(cons? '(1 2))");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "#t");

  result = eval_string("(cons? nil)");
  EXPECT_TRUE(result->is_nil());
}

TEST_F(EvaluatorTest, QuoteSpecialForm) {
  auto result = eval_string("'hello");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "hello");

  result = eval_string("'(1 2 3)");
  EXPECT_TRUE(result->is_cons());
  EXPECT_TRUE(result->car()->is_number());
  EXPECT_DOUBLE_EQ(result->car()->as_number(), 1.0);

  result = eval_string("(quote hello)");
  EXPECT_TRUE(result->is_symbol());
  EXPECT_EQ(result->as_symbol(), "hello");

  result = eval_string("(quote (+ 1 2))");
  EXPECT_TRUE(result->is_cons());
  EXPECT_TRUE(result->car()->is_symbol());
  EXPECT_EQ(result->car()->as_symbol(), "+");
}

TEST_F(EvaluatorTest, IfSpecialForm) {
  auto result = eval_string("(if #t 1 2)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 1.0);

  result = eval_string("(if nil 1 2)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 2.0);

  result = eval_string("(if (< 1 2) \"yes\" \"no\")");
  EXPECT_TRUE(result->is_string());
  EXPECT_EQ(result->as_string(), "yes");

  result = eval_string("(if (> 1 2) \"yes\" \"no\")");
  EXPECT_TRUE(result->is_string());
  EXPECT_EQ(result->as_string(), "no");

  result = eval_string("(if #t 42)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 42.0);

  result = eval_string("(if nil 42)");
  EXPECT_TRUE(result->is_nil());
}

TEST_F(EvaluatorTest, DefineSpecialForm) {
  eval_string("(define x 42)");
  auto result = eval_string("x");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 42.0);

  eval_string("(define y (+ 1 2 3))");
  result = eval_string("y");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 6.0);

  eval_string("(define message \"hello world\")");
  result = eval_string("message");
  EXPECT_TRUE(result->is_string());
  EXPECT_EQ(result->as_string(), "hello world");
}

TEST_F(EvaluatorTest, LambdaSpecialForm) {
  auto result = eval_string("((lambda (x) (+ x 1)) 5)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 6.0);

  result = eval_string("((lambda (x y) (+ x y)) 3 4)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 7.0);

  result = eval_string("((lambda () 42))");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 42.0);

  eval_string("(define square (lambda (x) (* x x)))");
  result = eval_string("(square 5)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 25.0);
}

TEST_F(EvaluatorTest, LexicalScoping) {
  eval_string("(define x 10)");
  eval_string("(define f (lambda (y) (+ x y)))");
  auto result = eval_string("(f 5)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 15.0);

  result = eval_string("((lambda (x) ((lambda (y) (+ x y)) 20)) 30)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 50.0);
}

TEST_F(EvaluatorTest, ComplexExpressions) {
  auto result = eval_string("(+ (* 2 3) (/ 8 2))");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 10.0);

  result = eval_string("(car (cdr '(1 2 3 4)))");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 2.0);

  result = eval_string("(cons (+ 1 2) (list 4 5))");
  EXPECT_TRUE(result->is_cons());
  EXPECT_TRUE(result->car()->is_number());
  EXPECT_DOUBLE_EQ(result->car()->as_number(), 3.0);
}

TEST_F(EvaluatorTest, FactorialExample) {
  std::string const factorial_def = R"(
        (define factorial
          (lambda (n)
            (if (= n 0)
                1
                (* n (factorial (- n 1))))))
    )";

  eval_string(factorial_def);

  auto result = eval_string("(factorial 0)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 1.0);

  result = eval_string("(factorial 1)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 1.0);

  result = eval_string("(factorial 5)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 120.0);
}

TEST_F(EvaluatorTest, ErrorConditions) {
  EXPECT_THROW(eval_string("undefined_symbol"), EvalError);
  EXPECT_THROW(eval_string("(+ 1 \"hello\")"), EvalError);
  EXPECT_THROW(eval_string("(car 42)"), EvalError);
  EXPECT_THROW(eval_string("(cdr \"hello\")"), EvalError);
  EXPECT_THROW(eval_string("(42 1 2)"), EvalError);
  EXPECT_THROW(eval_string("(< \"a\" \"b\")"), EvalError);
  EXPECT_THROW(eval_string("(define 42 100)"), EvalError);
  EXPECT_THROW(eval_string("(lambda (42) (+ 1 2))"), EvalError);
  EXPECT_THROW(eval_string("((lambda (x) x) 1 2)"), EvalError);
}

TEST_F(EvaluatorTest, ArgumentCountErrors) {
  EXPECT_THROW(eval_string("(car)"), EvalError);
  EXPECT_THROW(eval_string("(car 1 2)"), EvalError);
  EXPECT_THROW(eval_string("(cdr)"), EvalError);
  EXPECT_THROW(eval_string("(cdr 1 2)"), EvalError);
  EXPECT_THROW(eval_string("(cons 1)"), EvalError);
  EXPECT_THROW(eval_string("(cons 1 2 3)"), EvalError);
  EXPECT_THROW(eval_string("(= 1)"), EvalError);
  EXPECT_THROW(eval_string("(= 1 2 3)"), EvalError);
  EXPECT_THROW(eval_string("(< 1)"), EvalError);
  EXPECT_THROW(eval_string("(< 1 2 3)"), EvalError);
  EXPECT_THROW(eval_string("(null?)"), EvalError);
  EXPECT_THROW(eval_string("(null? 1 2)"), EvalError);
}

TEST_F(EvaluatorTest, SpecialFormErrors) {
  EXPECT_THROW(eval_string("(quote)"), EvalError);
  EXPECT_THROW(eval_string("(if)"), EvalError);
  EXPECT_THROW(eval_string("(if #t)"), EvalError);
  EXPECT_THROW(eval_string("(define)"), EvalError);
  EXPECT_THROW(eval_string("(define x)"), EvalError);
  EXPECT_THROW(eval_string("(lambda)"), EvalError);
  EXPECT_THROW(eval_string("(lambda ())"), EvalError);
}

TEST_F(EvaluatorTest, NestedEnvironments) {
  eval_string("(define x 100)");
  eval_string("(define outer (lambda (y) (lambda (z) (+ x y z))))");
  eval_string("(define inner (outer 10))");
  auto result = eval_string("(inner 1)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 111.0);
}

TEST_F(EvaluatorTest, CurriedFunctions) {
  eval_string("(define add (lambda (x) (lambda (y) (+ x y))))");
  eval_string("(define add5 (add 5))");
  auto result = eval_string("(add5 10)");
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 15.0);
}

class IOTest : public EvaluatorTest {
 protected:
  void SetUp() override {
    EvaluatorTest::SetUp();
    original_cout = std::cout.rdbuf(captured_output.rdbuf());
    original_cin = std::cin.rdbuf(mock_input.rdbuf());
  }

  void TearDown() override {
    std::cout.rdbuf(original_cout);
    std::cin.rdbuf(original_cin);
  }

  std::string get_output() {
    std::string output = captured_output.str();
    captured_output.str("");
    captured_output.clear();
    return output;
  }

  void set_input(std::string const& input) {
    mock_input.str(input);
    mock_input.clear();
  }

 private:
  std::ostringstream captured_output;
  std::istringstream mock_input;
  std::streambuf* original_cout = nullptr;
  std::streambuf* original_cin = nullptr;
};

TEST_F(IOTest, PrintFunction) {
  auto result = eval_string("(print \"hello\")");

  // Check return value
  EXPECT_TRUE(result->is_string());
  EXPECT_EQ(result->as_string(), "hello");

  // Check output
  std::string const output = get_output();
  EXPECT_EQ(output, "\"hello\"\n");
}

TEST_F(IOTest, PrintNumbers) {
  auto result = eval_string("(print 42)");

  // Check return value
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 42.0);

  // Check output
  std::string const output = get_output();
  EXPECT_EQ(output, "42\n");
}

TEST_F(IOTest, PrintNil) {
  auto result = eval_string("(print nil)");

  // Check return value
  EXPECT_TRUE(result->is_nil());

  // Check output
  std::string const output = get_output();
  EXPECT_EQ(output, "nil\n");
}

TEST_F(IOTest, DisplayFunction) {
  auto result = eval_string("(display \"hello\")");

  // Check return value
  EXPECT_TRUE(result->is_string());
  EXPECT_EQ(result->as_string(), "hello");

  // Check output (no newline)
  std::string const output = get_output();
  EXPECT_EQ(output, "\"hello\"");
}

TEST_F(IOTest, DisplayNumbers) {
  auto result = eval_string("(display 123)");

  // Check return value
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 123.0);

  // Check output
  std::string const output = get_output();
  EXPECT_EQ(output, "123");
}

TEST_F(IOTest, NewlineFunction) {
  auto result = eval_string("(newline)");

  // Check return value
  EXPECT_TRUE(result->is_nil());

  // Check output
  std::string const output = get_output();
  EXPECT_EQ(output, "\n");
}

TEST_F(IOTest, ReadLineFunction) {
  set_input("hello world\n");

  auto result = eval_string("(read-line)");

  // Check return value
  EXPECT_TRUE(result->is_string());
  EXPECT_EQ(result->as_string(), "hello world");
}

TEST_F(IOTest, ReadLineEmpty) {
  set_input("\n");

  auto result = eval_string("(read-line)");

  // Check return value - empty line should return empty string
  EXPECT_TRUE(result->is_string());
  EXPECT_EQ(result->as_string(), "");
}

TEST_F(IOTest, ReadLineEOF) {
  set_input("");  // No input, immediate EOF

  auto result = eval_string("(read-line)");

  // Check return value - EOF should return nil
  EXPECT_TRUE(result->is_nil());
}

TEST_F(IOTest, CombinedIOOperations) {
  // Test display followed by newline
  eval_string("(display \"Hello\")");
  eval_string("(display \" \")");
  eval_string("(display \"World\")");
  eval_string("(newline)");

  std::string const output = get_output();
  EXPECT_EQ(output, "\"Hello\"\" \"\"World\"\n");
}

TEST_F(IOTest, PrintInExpressions) {
  // Test that print can be used in larger expressions
  auto result = eval_string("(+ (print 5) (print 10))");

  // Check return value
  EXPECT_TRUE(result->is_number());
  EXPECT_DOUBLE_EQ(result->as_number(), 15.0);

  // Check output
  std::string const output = get_output();
  EXPECT_EQ(output, "5\n10\n");
}

TEST_F(EvaluatorTest, IOArgumentErrors) {
  // Test argument count validation
  EXPECT_THROW(eval_string("(print)"), EvalError);
  EXPECT_THROW(eval_string("(print 1 2)"), EvalError);
  EXPECT_THROW(eval_string("(display)"), EvalError);
  EXPECT_THROW(eval_string("(display 1 2)"), EvalError);
  EXPECT_THROW(eval_string("(newline 1)"), EvalError);
  EXPECT_THROW(eval_string("(read-line 1)"), EvalError);
}

}  // namespace lisp
