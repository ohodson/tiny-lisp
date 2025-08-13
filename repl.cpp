#include "repl.hpp"

#include <exception>
#include <iostream>
#include <string>

#include "evaluator.hpp"
#include "parser.hpp"
#include "tokenizer.hpp"
#include "value.hpp"

namespace lisp {

namespace {
void print_welcome() {
  std::cout << "Tiny Lisp Interpreter v1.0\n";
  std::cout << "Type expressions to evaluate, or 'quit' to exit.\n";
  std::cout << "Example: (+ 1 2 3)\n\n";
}

std::string read_input() {
  std::cout << "lisp> ";
  std::string line;
  std::getline(std::cin, line);
  return line;
}

void print_result(const ValuePtr& result) {
  std::cout << result->to_string() << '\n';
}

void print_error(const std::exception& exn) {
  std::cout << "Error: " << exn.what() << '\n';
}
}  // namespace

ValuePtr REPL::eval_string(const std::string& input) {
  if (input.empty()) {
    return nullptr;
  }

  Tokenizer tokenizer(input);
  auto tokens = tokenizer.tokenize();

  Parser parser(tokens);
  auto expressions = parser.parse_multiple();

  ValuePtr result = {};
  for (const auto& expr : expressions) {
    result = evaluator.eval(expr);
  }

  return result;
}

void REPL::run() {
  print_welcome();
  running = true;

  while (running) {
    try {
      std::string const input = read_input();

      // Handle special commands
      if (input == "quit" || input == "exit" || input == ":q") {
        stop();
        continue;
      }

      if (input.empty()) {
        continue;
      }

      ValuePtr const result = eval_string(input);
      if (result) {
        print_result(result);
      }

    } catch (const ParseError& e) {
      print_error(e);
    } catch (const EvalError& e) {
      print_error(e);
    } catch (const std::exception& e) {
      print_error(e);
    }
  }

  std::cout << "Goodbye!\n";
}

void REPL::stop() { running = false; }

}  // namespace lisp