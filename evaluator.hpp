#pragma once

#include <memory>
#include <stdexcept>

#include "value.hpp"

namespace lisp {

class EvalError : public std::runtime_error {
 public:
  explicit EvalError(const std::string& message)
      : std::runtime_error(message) {}
};

class Evaluator {
 private:
  std::shared_ptr<Environment> global_env;

  void setup_builtins();
  static ValuePtr do_quote(const ValuePtr& quote_args);
  ValuePtr do_if(const ValuePtr& if_args, Environment& env);
  ValuePtr do_define(const ValuePtr& define_args, Environment& env);
  static ValuePtr do_lambda(const ValuePtr& lambda_args, Environment& env);
  ValuePtr eval_list(const ValuePtr& expr, Environment& env);
  std::vector<ValuePtr> eval_args(ValuePtr args, Environment& env);

 public:
  Evaluator();
  ValuePtr eval(const ValuePtr& expr, Environment& env);
  ValuePtr eval(const ValuePtr& expr);
  std::shared_ptr<Environment> get_global_env() { return global_env; }
};

}  // namespace lisp