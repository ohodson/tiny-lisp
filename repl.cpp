#include "repl.hpp"

#include <exception>
#include <iostream>
#include <string>

#include "evaluator.hpp"
#include "parser.hpp"
#include "tokenizer.hpp"
#include "value.hpp"

namespace lisp {

REPL::REPL() : running(false) {}

void REPL::print_welcome() {
    std::cout << "Tiny Lisp Interpreter v1.0\n";
    std::cout << "Type expressions to evaluate, or 'quit' to exit.\n";
    std::cout << "Example: (+ 1 2 3)\n\n";
}

std::string REPL::read_input() {
    std::cout << "lisp> ";
    std::string line;
    std::getline(std::cin, line);
    return line;
}

void REPL::print_result(ValuePtr result) {
    std::cout << result->to_string() << std::endl;
}

void REPL::print_error(const std::exception& e) {
    std::cout << "Error: " << e.what() << std::endl;
}

ValuePtr REPL::eval_string(const std::string& input) {
    if (input.empty()) {
        return nullptr;
    }
    
    Tokenizer tokenizer(input);
    auto tokens = tokenizer.tokenize();
    
    Parser parser(tokens);
    auto expressions = parser.parse_multiple();
    
    ValuePtr result = nullptr;
    for (auto expr : expressions) {
        result = evaluator.eval(expr);
    }
    
    return result;
}

void REPL::run() {
    print_welcome();
    running = true;
    
    while (running) {
        try {
            std::string input = read_input();
            
            // Handle special commands
            if (input == "quit" || input == "exit" || input == ":q") {
                stop();
                continue;
            }
            
            if (input.empty()) {
                continue;
            }
            
            ValuePtr result = eval_string(input);
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

void REPL::stop() {
    running = false;
}

}  // namespace lisp