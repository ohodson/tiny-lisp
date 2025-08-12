#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace lisp {

class Environment;
struct Value;

using ValuePtr = std::shared_ptr<Value>;
using BuiltinFunction =
    std::function<ValuePtr(const std::vector<ValuePtr>&, Environment&)>;

enum class ValueType { NIL, NUMBER, STRING, SYMBOL, CONS, BUILTIN, LAMBDA };

struct Lambda {
  std::vector<std::string> params;
  ValuePtr body;
  std::shared_ptr<Environment> closure;
};

struct Value : public std::enable_shared_from_this<Value> {
  ValueType type;
  std::variant<std::nullptr_t,                 // NIL
               double,                         // NUMBER
               std::string,                    // STRING or SYMBOL
               std::pair<ValuePtr, ValuePtr>,  // CONS
               BuiltinFunction,                // BUILTIN
               Lambda                          // LAMBDA
               >
      data;

  Value(ValueType t) : type(t) {
    switch (t) {
      case ValueType::NIL:
        data = nullptr;
        break;
      default:
        break;
    }
  }

  Value(double n) : type(ValueType::NUMBER), data(n) {}
  Value(const std::string& s, ValueType t = ValueType::STRING)
      : type(t), data(s) {}
  Value(ValuePtr car, ValuePtr cdr)
      : type(ValueType::CONS), data(std::make_pair(car, cdr)) {}
  Value(BuiltinFunction func) : type(ValueType::BUILTIN), data(func) {}
  Value(const Lambda& lambda) : type(ValueType::LAMBDA), data(lambda) {}

  bool is_nil() const { return type == ValueType::NIL; }
  bool is_number() const { return type == ValueType::NUMBER; }
  bool is_string() const { return type == ValueType::STRING; }
  bool is_symbol() const { return type == ValueType::SYMBOL; }
  bool is_cons() const { return type == ValueType::CONS; }
  bool is_builtin() const { return type == ValueType::BUILTIN; }
  bool is_lambda() const { return type == ValueType::LAMBDA; }

  double as_number() const { return std::get<double>(data); }
  const std::string& as_string() const { return std::get<std::string>(data); }
  const std::string& as_symbol() const { return std::get<std::string>(data); }
  const std::pair<ValuePtr, ValuePtr>& as_cons() const {
    return std::get<std::pair<ValuePtr, ValuePtr>>(data);
  }
  const BuiltinFunction& as_builtin() const {
    return std::get<BuiltinFunction>(data);
  }
  const Lambda& as_lambda() const { return std::get<Lambda>(data); }

  ValuePtr car() const {
    if (!is_cons()) return nullptr;
    return as_cons().first;
  }

  ValuePtr cdr() const {
    if (!is_cons()) return nullptr;
    return as_cons().second;
  }

  std::string to_string() const;
};

class Environment : public std::enable_shared_from_this<Environment> {
 private:
  std::map<std::string, ValuePtr> bindings;
  std::shared_ptr<Environment> parent;

 public:
  Environment(std::shared_ptr<Environment> p = nullptr) : parent(p) {}

  void define(const std::string& name, ValuePtr value) {
    bindings[name] = value;
  }

  ValuePtr lookup(const std::string& name) {
    auto it = bindings.find(name);
    if (it != bindings.end()) {
      return it->second;
    }
    if (parent) {
      return parent->lookup(name);
    }
    return nullptr;
  }

  std::shared_ptr<Environment> extend() {
    return std::make_shared<Environment>(shared_from_this());
  }
};

ValuePtr make_nil();
ValuePtr make_number(double n);
ValuePtr make_string(const std::string& s);
ValuePtr make_symbol(const std::string& s);
ValuePtr make_cons(ValuePtr car, ValuePtr cdr);
ValuePtr make_builtin(BuiltinFunction func);
ValuePtr make_lambda(const std::vector<std::string>& params, ValuePtr body,
                     std::shared_ptr<Environment> closure);

}  // namespace lisp