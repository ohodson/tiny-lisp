#include "value.hpp"

#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace lisp {

std::string Value::to_string() const {
  switch (type) {
    case ValueType::NIL:
      return "nil";
    case ValueType::NUMBER: {
      double const number = as_number();
      if (number == static_cast<int>(number)) {
        return std::to_string(static_cast<int>(number));
      }
      return std::to_string(number);
    }
    case ValueType::STRING:
      return "\"" + as_string() + "\"";
    case ValueType::SYMBOL:
      return as_symbol();
    case ValueType::CONS: {
      std::ostringstream oss;
      oss << "(";
      ValuePtr current = std::const_pointer_cast<Value>(shared_from_this());
      bool first = true;

      while (current && current->is_cons()) {
        if (!first) {
          oss << " ";
        }
        first = false;

        oss << current->car()->to_string();
        current = current->cdr();

        if (current && !current->is_nil() && !current->is_cons()) {
          oss << " . " << current->to_string();
          break;
        }
      }
      oss << ")";
      return oss.str();
    }
    case ValueType::BUILTIN:
      return "#<builtin>";
    case ValueType::LAMBDA:
      return "#<lambda>";
    default:
      return "#<unknown>";
  }
}

ValuePtr make_nil() { return std::make_shared<Value>(ValueType::NIL); }

ValuePtr make_number(double n) { return std::make_shared<Value>(n); }

ValuePtr make_string(const std::string& text) {
  return std::make_shared<Value>(text, ValueType::STRING);
}

ValuePtr make_symbol(const std::string& symbol) {
  return std::make_shared<Value>(symbol, ValueType::SYMBOL);
}

ValuePtr make_cons(ValuePtr car, ValuePtr cdr) {
  return std::make_shared<Value>(car, cdr);
}

ValuePtr make_builtin(const BuiltinFunction& func) {
  return std::make_shared<Value>(func);
}

ValuePtr make_lambda(const std::vector<std::string>& params,
                     const std::vector<ValuePtr>& body,
                     std::shared_ptr<Environment>&& closure) {
  Lambda const lambda{params, body, std::move(closure)};
  return std::make_shared<Value>(lambda);
}

}  // namespace lisp