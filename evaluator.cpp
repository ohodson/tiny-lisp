#include "evaluator.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "value.hpp"

namespace lisp {

namespace {

bool is_self_evaluating(const ValuePtr& expr) {
  return expr->is_number() || expr->is_string() || expr->is_nil();
}

//
// Builtin arithmetic functions
//

ValuePtr builtin_add(const std::vector<ValuePtr>& args, Environment& /*env*/) {
  double sum = 0.0;
  for (const auto& arg : args) {
    if (!arg->is_number()) {
      throw EvalError("+ requires numeric arguments");
    }
    sum += arg->as_number();
  }
  return make_number(sum);
}

ValuePtr builtin_subtract(const std::vector<ValuePtr>& args,
                          Environment& /*env*/) {
  if (args.empty()) {
    throw EvalError("- requires at least one argument");
  }
  if (!args[0]->is_number()) {
    throw EvalError("- requires numeric arguments");
  }

  double result = args[0]->as_number();
  for (size_t i = 1; i < args.size(); ++i) {
    if (!args[i]->is_number()) {
      throw EvalError("- requires numeric arguments");
    }
    result -= args[i]->as_number();
  }
  return make_number(result);
}

ValuePtr builtin_multiply(const std::vector<ValuePtr>& args,
                          Environment& /*env*/) {
  if (args.empty()) {
    throw EvalError("* requires at least one argument");
  }
  double product = 1.0;
  for (const auto& arg : args) {
    if (!arg->is_number()) {
      throw EvalError("* requires numeric arguments");
    }
    product *= arg->as_number();
  }
  return make_number(product);
}

ValuePtr builtin_divide(const std::vector<ValuePtr>& args,
                        Environment& /*env*/) {
  if (args.empty()) {
    throw EvalError("/ requires at least one argument");
  }
  if (!args[0]->is_number()) {
    throw EvalError("/ requires numeric arguments");
  }

  double result = args[0]->as_number();
  for (size_t i = 1; i < args.size(); ++i) {
    if (!args[i]->is_number()) {
      throw EvalError("/ requires numeric arguments");
    }
    if (args[i]->as_number() == 0) {
      throw EvalError("Division by zero");
    }
    result /= args[i]->as_number();
  }
  return make_number(result);
}

//
// Builtin list functions
//

ValuePtr builtin_car(const std::vector<ValuePtr>& args, Environment& /*env*/) {
  if (args.size() != 1) {
    throw EvalError("car requires exactly one argument");
  }
  if (args[0]->is_nil()) {
    return make_nil();
  }
  if (!args[0]->is_cons()) {
    throw EvalError("car requires a list argument");
  }
  return args[0]->car();
}

ValuePtr builtin_cdr(const std::vector<ValuePtr>& args, Environment& /*env*/) {
  if (args.size() != 1) {
    throw EvalError("cdr requires exactly one argument");
  }
  if (args[0]->is_nil()) {
    return make_nil();
  }
  if (!args[0]->is_cons()) {
    throw EvalError("cdr requires a list argument");
  }
  return args[0]->cdr();
}

ValuePtr builtin_cons(const std::vector<ValuePtr>& args, Environment& /*env*/) {
  if (args.size() != 2) {
    throw EvalError("cons requires exactly two arguments");
  }
  return make_cons(args[0], args[1]);
}

ValuePtr builtin_list(const std::vector<ValuePtr>& args, Environment& /*env*/) {
  ValuePtr result = make_nil();
  for (auto it = args.rbegin(); it != args.rend(); ++it) {
    result = make_cons(*it, result);
  }
  return result;
}

//
// Builtin comparison operations
//

ValuePtr builtin_equals(const std::vector<ValuePtr>& args,
                        Environment& /*env*/) {
  if (args.size() != 2) {
    throw EvalError("= requires exactly two arguments");
  }

  const ValuePtr& lhs = args[0];
  const ValuePtr& rhs = args[1];

  if (lhs->type != rhs->type) {
    return make_nil();
  }

  if (lhs->is_number()) {
    return lhs->as_number() == rhs->as_number() ? make_symbol("#t")
                                                : make_nil();
  }

  if (lhs->is_string()) {
    return lhs->as_string() == rhs->as_string() ? make_symbol("#t")
                                                : make_nil();
  }

  if (lhs->is_symbol()) {
    return lhs->as_symbol() == rhs->as_symbol() ? make_symbol("#t")
                                                : make_nil();
  }

  if (lhs->is_nil() && rhs->is_nil()) {
    return make_symbol("#t");
  }

  return make_nil();
}

ValuePtr builtin_less_than(const std::vector<ValuePtr>& args,
                           Environment& /*env*/) {
  if (args.size() != 2) {
    throw EvalError("< requires exactly two arguments");
  }
  if (!args[0]->is_number() || !args[1]->is_number()) {
    throw EvalError("< requires numeric arguments");
  }
  return args[0]->as_number() < args[1]->as_number() ? make_symbol("#t")
                                                     : make_nil();
}

ValuePtr builtin_greater_than(const std::vector<ValuePtr>& args,
                              Environment& /*env*/) {
  if (args.size() != 2) {
    throw EvalError("> requires exactly two arguments");
  }
  if (!args[0]->is_number() || !args[1]->is_number()) {
    throw EvalError("> requires numeric arguments");
  }
  return args[0]->as_number() > args[1]->as_number() ? make_symbol("#t")
                                                     : make_nil();
}

//
// Builtin type predicates
//

ValuePtr builtin_is_null(const std::vector<ValuePtr>& args,
                         Environment& /*env*/) {
  if (args.size() != 1) {
    throw EvalError("null? requires exactly one argument");
  }
  return args[0]->is_nil() ? make_symbol("#t") : make_nil();
}

ValuePtr builtin_is_number(const std::vector<ValuePtr>& args,
                           Environment& /*env*/) {
  if (args.size() != 1) {
    throw EvalError("number? requires exactly one argument");
  }
  return args[0]->is_number() ? make_symbol("#t") : make_nil();
}

ValuePtr builtin_is_string(const std::vector<ValuePtr>& args,
                           Environment& /*env*/) {
  if (args.size() != 1) {
    throw EvalError("string? requires exactly one argument");
  }
  return args[0]->is_string() ? make_symbol("#t") : make_nil();
}

ValuePtr builtin_is_symbol(const std::vector<ValuePtr>& args,
                           Environment& /*env*/) {
  if (args.size() != 1) {
    throw EvalError("symbol? requires exactly one argument");
  }
  return args[0]->is_symbol() ? make_symbol("#t") : make_nil();
}

ValuePtr builtin_is_cons(const std::vector<ValuePtr>& args,
                         Environment& /*env*/) {
  if (args.size() != 1) {
    throw EvalError("cons? requires exactly one argument");
  }
  return args[0]->is_cons() ? make_symbol("#t") : make_nil();
}

}  // namespace

Evaluator::Evaluator() {
  global_env = std::make_shared<Environment>();
  setup_builtins();
}

ValuePtr Evaluator::eval(const ValuePtr& expr, Environment& env) {
  if (!expr) {
    throw EvalError("Cannot evaluate null expression");
  }

  // Self-evaluating expressions
  if (is_self_evaluating(expr)) {
    return expr;
  }

  // Symbols - variable lookup
  if (expr->is_symbol()) {
    ValuePtr value = env.lookup(expr->as_symbol());
    if (!value) {
      throw EvalError("Unbound symbol: " + expr->as_symbol());
    }
    return value;
  }

  // Lists - function calls or special forms
  if (expr->is_cons()) {
    return eval_list(expr, env);
  }

  throw EvalError("Cannot evaluate expression: " + expr->to_string());
}

ValuePtr Evaluator::eval(const ValuePtr& expr) {
  return eval(expr, *global_env);
}

ValuePtr Evaluator::do_quote(const ValuePtr& quote_args) {
  if (!quote_args->is_cons()) {
    throw EvalError("quote requires exactly one argument");
  }
  return quote_args->car();
}

ValuePtr Evaluator::do_if(const ValuePtr& if_args, Environment& env) {
  if (!if_args->is_cons() || !if_args->cdr()->is_cons()) {
    throw EvalError("if requires at least 2 arguments");
  }

  ValuePtr const condition = eval(if_args->car(), env);
  if (!condition->is_nil()) {
    return eval(if_args->cdr()->car(), env);
  }

  ValuePtr const else_branch = if_args->cdr()->cdr()->is_cons()
                                   ? if_args->cdr()->cdr()->car()
                                   : make_nil();
  return eval(else_branch, env);
}

ValuePtr Evaluator::do_define(const ValuePtr& define_args, Environment& env) {
  if (!define_args->is_cons() || !define_args->cdr()->is_cons()) {
    throw EvalError("define requires exactly 2 arguments");
  }

  ValuePtr const name_expr = define_args->car();
  ValuePtr const value_expr = define_args->cdr()->car();

  if (!name_expr->is_symbol()) {
    throw EvalError("define requires a symbol as first argument");
  }

  ValuePtr value = eval(value_expr, env);
  env.define(name_expr->as_symbol(), value);
  return value;
}

ValuePtr Evaluator::do_lambda(const ValuePtr& lambda_args, Environment& env) {
  if (!lambda_args->is_cons() || !lambda_args->cdr()->is_cons()) {
    throw EvalError("lambda requires at least 2 arguments");
  }

  ValuePtr const params_list = lambda_args->car();
  ValuePtr body = lambda_args->cdr()->car();

  std::vector<std::string> params;
  ValuePtr current = params_list;
  while (current && current->is_cons()) {
    ValuePtr const param = current->car();
    if (!param->is_symbol()) {
      throw EvalError("lambda parameter must be a symbol");
    }
    params.push_back(param->as_symbol());
    current = current->cdr();
  }

  return make_lambda(params, body,
                     std::make_shared<Environment>(env.shared_from_this()));
}

ValuePtr Evaluator::eval_list(const ValuePtr& expr, Environment& env) {
  if (!expr->is_cons()) {
    throw EvalError("Expected list for function call");
  }

  ValuePtr const first = expr->car();
  ValuePtr const args = expr->cdr();

  if (!first) {
    throw EvalError("Empty function call");
  }

  // Special forms
  if (first->is_symbol()) {
    const std::string& symbol = first->as_symbol();

    if (symbol == "quote") {
      return do_quote(args);
    }

    if (symbol == "if") {
      return do_if(args, env);
    }

    if (symbol == "define") {
      return do_define(args, env);
    }

    if (symbol == "lambda") {
      return do_lambda(args, env);
    }
  }

  // Function call
  ValuePtr const func = eval(first, env);
  std::vector<ValuePtr> arg_values = eval_args(args, env);

  if (func->is_builtin()) {
    return func->as_builtin()(arg_values, env);
  }

  if (func->is_lambda()) {
    const Lambda& lambda = func->as_lambda();

    if (arg_values.size() != lambda.params.size()) {
      throw EvalError("Lambda expects " + std::to_string(lambda.params.size()) +
                      " arguments, got " + std::to_string(arg_values.size()));
    }

    auto new_env = lambda.closure->extend();
    for (size_t i = 0; i < lambda.params.size(); ++i) {
      new_env->define(lambda.params[i], arg_values[i]);
    }

    return eval(lambda.body, *new_env);
  }

  throw EvalError("Cannot call non-function: " + func->to_string());
}

std::vector<ValuePtr> Evaluator::eval_args(ValuePtr args, Environment& env) {
  std::vector<ValuePtr> result;
  ValuePtr current = std::move(args);

  while (current && current->is_cons()) {
    result.push_back(eval(current->car(), env));
    current = current->cdr();
  }
  return result;
}

void Evaluator::setup_builtins() {
  // Boolean constants
  global_env->define("#t", make_symbol("#t"));
  global_env->define("#f", make_symbol("#f"));

  // Arithmetic operations
  global_env->define("+", make_builtin(builtin_add));
  global_env->define("-", make_builtin(builtin_subtract));
  global_env->define("*", make_builtin(builtin_multiply));
  global_env->define("/", make_builtin(builtin_divide));

  // List operations
  global_env->define("car", make_builtin(builtin_car));
  global_env->define("cdr", make_builtin(builtin_cdr));
  global_env->define("cons", make_builtin(builtin_cons));
  global_env->define("list", make_builtin(builtin_list));

  // Comparison operations
  global_env->define("=", make_builtin(builtin_equals));
  global_env->define("<", make_builtin(builtin_less_than));
  global_env->define(">", make_builtin(builtin_greater_than));

  // Type predicates
  global_env->define("null?", make_builtin(builtin_is_null));
  global_env->define("number?", make_builtin(builtin_is_number));
  global_env->define("string?", make_builtin(builtin_is_string));
  global_env->define("symbol?", make_builtin(builtin_is_symbol));
  global_env->define("cons?", make_builtin(builtin_is_cons));
}

}  // namespace lisp