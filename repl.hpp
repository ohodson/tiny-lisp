#pragma once

#include <string>

#include "evaluator.hpp"
#include "value.hpp"

namespace lisp {

class REPL {
private:
    Evaluator evaluator;
    bool running;
    
    void print_welcome();
    std::string read_input();
    void print_result(ValuePtr result);
    void print_error(const std::exception& e);
    
public:
    REPL();
    void run();
    void stop();
    
    // For non-interactive evaluation
    ValuePtr eval_string(const std::string& input);
};

}  // namespace lisp