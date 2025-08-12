#pragma once

#include <stdexcept>
#include <memory>

#include "value.hpp"

namespace lisp {

class EvalError : public std::runtime_error {
 public:
  EvalError(const std::string& message) : std::runtime_error(message) {}
};

class Evaluator {
 private:
  std::shared_ptr<Environment> global_env;

  void setup_builtins();
  ValuePtr eval_list(ValuePtr expr, Environment& env);
  std::vector<ValuePtr> eval_args(ValuePtr args, Environment& env);
  bool is_self_evaluating(ValuePtr expr);

 public:
  Evaluator();
  ValuePtr eval(ValuePtr expr, Environment& env);
  ValuePtr eval(ValuePtr expr);
  std::shared_ptr<Environment> get_global_env() { return global_env; }
};

}  // namespace lisp