#include "evaluator.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "value.hpp"

namespace lisp {

Evaluator::Evaluator() {
  global_env = std::make_shared<Environment>();
  setup_builtins();
}

bool Evaluator::is_self_evaluating(ValuePtr expr) {
  return expr->is_number() || expr->is_string() || expr->is_nil();
}

ValuePtr Evaluator::eval(ValuePtr expr, Environment& env) {
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

ValuePtr Evaluator::eval(ValuePtr expr) { return eval(expr, *global_env); }

ValuePtr Evaluator::eval_list(ValuePtr expr, Environment& env) {
  if (!expr->is_cons()) {
    throw EvalError("Expected list for function call");
  }

  ValuePtr first = expr->car();
  ValuePtr args = expr->cdr();

  if (!first) {
    throw EvalError("Empty function call");
  }

  // Special forms
  if (first->is_symbol()) {
    const std::string& symbol = first->as_symbol();

    if (symbol == "quote") {
      if (!args->is_cons()) {
        throw EvalError("quote requires exactly one argument");
      }
      return args->car();
    }

    if (symbol == "if") {
      if (!args->is_cons() || !args->cdr()->is_cons()) {
        throw EvalError("if requires at least 2 arguments");
      }

      ValuePtr condition = eval(args->car(), env);
      ValuePtr then_branch = args->cdr()->car();
      ValuePtr else_branch = args->cdr()->cdr()->is_cons()
                                 ? args->cdr()->cdr()->car()
                                 : make_nil();

      if (!condition->is_nil()) {
        return eval(then_branch, env);
      } else {
        return eval(else_branch, env);
      }
    }

    if (symbol == "define") {
      if (!args->is_cons() || !args->cdr()->is_cons()) {
        throw EvalError("define requires exactly 2 arguments");
      }

      ValuePtr name_expr = args->car();
      ValuePtr value_expr = args->cdr()->car();

      if (!name_expr->is_symbol()) {
        throw EvalError("define requires a symbol as first argument");
      }

      ValuePtr value = eval(value_expr, env);
      env.define(name_expr->as_symbol(), value);
      return value;
    }

    if (symbol == "lambda") {
      if (!args->is_cons() || !args->cdr()->is_cons()) {
        throw EvalError("lambda requires at least 2 arguments");
      }

      ValuePtr params_list = args->car();
      ValuePtr body = args->cdr()->car();

      std::vector<std::string> params;
      ValuePtr current = params_list;
      while (current && current->is_cons()) {
        ValuePtr param = current->car();
        if (!param->is_symbol()) {
          throw EvalError("lambda parameter must be a symbol");
        }
        params.push_back(param->as_symbol());
        current = current->cdr();
      }

      return make_lambda(params, body,
                         std::make_shared<Environment>(env.shared_from_this()));
    }
  }

  // Function call
  ValuePtr func = eval(first, env);
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
  ValuePtr current = args;

  while (current && current->is_cons()) {
    result.push_back(eval(current->car(), env));
    current = current->cdr();
  }

  return result;
}

void Evaluator::setup_builtins() {
  // Arithmetic operations
  global_env->define("+", make_builtin([](const std::vector<ValuePtr>& args,
                                          Environment&) -> ValuePtr {
                       double sum = 0.0;
                       for (auto arg : args) {
                         if (!arg->is_number()) {
                           throw EvalError("+ requires numeric arguments");
                         }
                         sum += arg->as_number();
                       }
                       return make_number(sum);
                     }));

  global_env->define("-", make_builtin([](const std::vector<ValuePtr>& args,
                                          Environment&) -> ValuePtr {
                       if (args.empty()) {
                         throw EvalError("- requires at least one argument");
                       }
                       if (!args[0]->is_number()) {
                         throw EvalError("- requires numeric arguments");
                       }

                       if (args.size() == 1) {
                         return make_number(-args[0]->as_number());
                       }

                       double result = args[0]->as_number();
                       for (size_t i = 1; i < args.size(); ++i) {
                         if (!args[i]->is_number()) {
                           throw EvalError("- requires numeric arguments");
                         }
                         result -= args[i]->as_number();
                       }
                       return make_number(result);
                     }));

  global_env->define("*", make_builtin([](const std::vector<ValuePtr>& args,
                                          Environment&) -> ValuePtr {
                       double product = 1.0;
                       for (auto arg : args) {
                         if (!arg->is_number()) {
                           throw EvalError("* requires numeric arguments");
                         }
                         product *= arg->as_number();
                       }
                       return make_number(product);
                     }));

  global_env->define("/", make_builtin([](const std::vector<ValuePtr>& args,
                                          Environment&) -> ValuePtr {
                       if (args.empty()) {
                         throw EvalError("/ requires at least one argument");
                       }
                       if (!args[0]->is_number()) {
                         throw EvalError("/ requires numeric arguments");
                       }

                       if (args.size() == 1) {
                         return make_number(1.0 / args[0]->as_number());
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
                     }));

  // List operations
  global_env->define("car", make_builtin([](const std::vector<ValuePtr>& args,
                                            Environment&) -> ValuePtr {
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
                     }));

  global_env->define("cdr", make_builtin([](const std::vector<ValuePtr>& args,
                                            Environment&) -> ValuePtr {
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
                     }));

  global_env->define("cons", make_builtin([](const std::vector<ValuePtr>& args,
                                             Environment&) -> ValuePtr {
                       if (args.size() != 2) {
                         throw EvalError("cons requires exactly two arguments");
                       }
                       return make_cons(args[0], args[1]);
                     }));

  global_env->define("list", make_builtin([](const std::vector<ValuePtr>& args,
                                             Environment&) -> ValuePtr {
                       ValuePtr result = make_nil();
                       for (int i = static_cast<int>(args.size()) - 1; i >= 0;
                            --i) {
                         result = make_cons(args[i], result);
                       }
                       return result;
                     }));

  // Comparison operations
  global_env->define(
      "=", make_builtin(
               [](const std::vector<ValuePtr>& args, Environment&) -> ValuePtr {
                 if (args.size() != 2) {
                   throw EvalError("= requires exactly two arguments");
                 }

                 ValuePtr a = args[0];
                 ValuePtr b = args[1];

                 if (a->type != b->type) {
                   return make_nil();
                 }

                 if (a->is_number()) {
                   return a->as_number() == b->as_number() ? make_symbol("#t")
                                                           : make_nil();
                 }

                 if (a->is_string()) {
                   return a->as_string() == b->as_string() ? make_symbol("#t")
                                                           : make_nil();
                 }

                 if (a->is_symbol()) {
                   return a->as_symbol() == b->as_symbol() ? make_symbol("#t")
                                                           : make_nil();
                 }

                 if (a->is_nil() && b->is_nil()) {
                   return make_symbol("#t");
                 }

                 return make_nil();
               }));

  global_env->define("<", make_builtin([](const std::vector<ValuePtr>& args,
                                          Environment&) -> ValuePtr {
                       if (args.size() != 2) {
                         throw EvalError("< requires exactly two arguments");
                       }
                       if (!args[0]->is_number() || !args[1]->is_number()) {
                         throw EvalError("< requires numeric arguments");
                       }
                       return args[0]->as_number() < args[1]->as_number()
                                  ? make_symbol("#t")
                                  : make_nil();
                     }));

  global_env->define(">", make_builtin([](const std::vector<ValuePtr>& args,
                                          Environment&) -> ValuePtr {
                       if (args.size() != 2) {
                         throw EvalError("> requires exactly two arguments");
                       }
                       if (!args[0]->is_number() || !args[1]->is_number()) {
                         throw EvalError("> requires numeric arguments");
                       }
                       return args[0]->as_number() > args[1]->as_number()
                                  ? make_symbol("#t")
                                  : make_nil();
                     }));

  // Type predicates
  global_env->define("null?", make_builtin([](const std::vector<ValuePtr>& args,
                                              Environment&) -> ValuePtr {
                       if (args.size() != 1) {
                         throw EvalError("null? requires exactly one argument");
                       }
                       return args[0]->is_nil() ? make_symbol("#t")
                                                : make_nil();
                     }));

  global_env->define(
      "number?", make_builtin([](const std::vector<ValuePtr>& args,
                                 Environment&) -> ValuePtr {
        if (args.size() != 1) {
          throw EvalError("number? requires exactly one argument");
        }
        return args[0]->is_number() ? make_symbol("#t") : make_nil();
      }));

  global_env->define(
      "symbol?", make_builtin([](const std::vector<ValuePtr>& args,
                                 Environment&) -> ValuePtr {
        if (args.size() != 1) {
          throw EvalError("symbol? requires exactly one argument");
        }
        return args[0]->is_symbol() ? make_symbol("#t") : make_nil();
      }));
}

}  // namespace lisp