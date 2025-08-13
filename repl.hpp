#pragma once

#include <string>

#include "evaluator.hpp"
#include "value.hpp"

namespace lisp {

class REPL {
private:
    Evaluator evaluator;
    bool running = false;

public:
    REPL() = default;

    void run();
    void stop();

    // For non-interactive evaluation
    ValuePtr eval_string(const std::string& input);
};

}  // namespace lisp