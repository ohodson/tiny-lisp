#include "value.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <utility>
#include <vector>

namespace lisp {

class ValueTest : public ::testing::Test {
 protected:
  void SetUp() override { env = std::make_shared<Environment>(); }

  // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
  std::shared_ptr<Environment> env;
};

TEST_F(ValueTest, NilValue) {
  auto nil_val = make_nil();
  EXPECT_TRUE(nil_val->is_nil());
  EXPECT_FALSE(nil_val->is_number());
  EXPECT_FALSE(nil_val->is_string());
  EXPECT_FALSE(nil_val->is_symbol());
  EXPECT_FALSE(nil_val->is_cons());
  EXPECT_FALSE(nil_val->is_builtin());
  EXPECT_FALSE(nil_val->is_lambda());
  EXPECT_EQ(nil_val->type, ValueType::NIL);
}

TEST_F(ValueTest, NumberValue) {
  constexpr double kTestNumber = 42.5;
  auto num_val = make_number(kTestNumber);
  EXPECT_FALSE(num_val->is_nil());
  EXPECT_TRUE(num_val->is_number());
  EXPECT_FALSE(num_val->is_string());
  EXPECT_FALSE(num_val->is_symbol());
  EXPECT_FALSE(num_val->is_cons());
  EXPECT_FALSE(num_val->is_builtin());
  EXPECT_FALSE(num_val->is_lambda());
  EXPECT_EQ(num_val->type, ValueType::NUMBER);
  EXPECT_DOUBLE_EQ(num_val->as_number(), kTestNumber);
}

TEST_F(ValueTest, StringValue) {
  const char* kTestString = "hello world";
  auto str_val = make_string(kTestString);
  EXPECT_FALSE(str_val->is_nil());
  EXPECT_FALSE(str_val->is_number());
  EXPECT_TRUE(str_val->is_string());
  EXPECT_FALSE(str_val->is_symbol());
  EXPECT_FALSE(str_val->is_cons());
  EXPECT_FALSE(str_val->is_builtin());
  EXPECT_FALSE(str_val->is_lambda());
  EXPECT_EQ(str_val->type, ValueType::STRING);
  EXPECT_EQ(str_val->as_string(), kTestString);
}

TEST_F(ValueTest, SymbolValue) {
  const char* kTestSymbol = "foo";
  auto sym_val = make_symbol(kTestSymbol);
  EXPECT_FALSE(sym_val->is_nil());
  EXPECT_FALSE(sym_val->is_number());
  EXPECT_FALSE(sym_val->is_string());
  EXPECT_TRUE(sym_val->is_symbol());
  EXPECT_FALSE(sym_val->is_cons());
  EXPECT_FALSE(sym_val->is_builtin());
  EXPECT_FALSE(sym_val->is_lambda());
  EXPECT_EQ(sym_val->type, ValueType::SYMBOL);
  EXPECT_EQ(sym_val->as_symbol(), kTestSymbol);
}

TEST_F(ValueTest, ConsValue) {
  auto car_val = make_number(1);
  auto cdr_val = make_number(2);
  auto cons_val = make_cons(car_val, cdr_val);

  EXPECT_FALSE(cons_val->is_nil());
  EXPECT_FALSE(cons_val->is_number());
  EXPECT_FALSE(cons_val->is_string());
  EXPECT_FALSE(cons_val->is_symbol());
  EXPECT_TRUE(cons_val->is_cons());
  EXPECT_FALSE(cons_val->is_builtin());
  EXPECT_FALSE(cons_val->is_lambda());
  EXPECT_EQ(cons_val->type, ValueType::CONS);

  EXPECT_EQ(cons_val->car(), car_val);
  EXPECT_EQ(cons_val->cdr(), cdr_val);
  EXPECT_DOUBLE_EQ(cons_val->car()->as_number(), 1.0);
  EXPECT_DOUBLE_EQ(cons_val->cdr()->as_number(), 2.0);
}

TEST_F(ValueTest, BuiltinValue) {
  auto builtin_func = [](const std::vector<ValuePtr>& /*args*/,
                         Environment& /*env*/) -> ValuePtr {
    return make_string("builtin");
  };
  ValuePtr const builtin_val = make_builtin(builtin_func);

  EXPECT_EQ(builtin_val->type, ValueType::BUILTIN);
  EXPECT_TRUE(builtin_val->is_builtin());
  EXPECT_FALSE(builtin_val->is_nil() || builtin_val->is_number() ||
               builtin_val->is_string() || builtin_val->is_symbol() ||
               builtin_val->is_cons() || builtin_val->is_lambda());

  std::vector<ValuePtr> const args;
  ValuePtr const result = builtin_val->as_builtin()(args, *env);
  EXPECT_TRUE(result->is_string());
  EXPECT_EQ(result->as_string(), "builtin");
}

TEST_F(ValueTest, LambdaValue) {
  std::vector<std::string> const params = {"x", "y"};
  std::vector<ValuePtr> body = {make_symbol("+")};
  auto closure = std::make_shared<Environment>();
  auto lambda_val = make_lambda(params, body, std::move(closure));

  EXPECT_FALSE(lambda_val->is_nil());
  EXPECT_FALSE(lambda_val->is_number());
  EXPECT_FALSE(lambda_val->is_string());
  EXPECT_FALSE(lambda_val->is_symbol());
  EXPECT_FALSE(lambda_val->is_cons());
  EXPECT_FALSE(lambda_val->is_builtin());
  EXPECT_TRUE(lambda_val->is_lambda());
  EXPECT_EQ(lambda_val->type, ValueType::LAMBDA);

  const auto& lambda_data = lambda_val->as_lambda();
  EXPECT_EQ(lambda_data.params.size(), 2);
  EXPECT_EQ(lambda_data.params[0], "x");
  EXPECT_EQ(lambda_data.params[1], "y");
  EXPECT_EQ(lambda_data.body, body);
  EXPECT_NE(lambda_data.closure, nullptr);
}

TEST_F(ValueTest, ConsCarCdrWithNil) {
  auto nil_val = make_nil();
  EXPECT_EQ(nil_val->car(), nullptr);
  EXPECT_EQ(nil_val->cdr(), nullptr);
}

class EnvironmentTest : public ::testing::Test {
 protected:
  void SetUp() override { env = std::make_shared<Environment>(); }

  // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
  std::shared_ptr<Environment> env;
};

TEST_F(EnvironmentTest, DefineAndLookup) {
  constexpr double kTestNumber = 42.0;
  auto val = make_number(kTestNumber);
  env->define("x", val);

  auto result = env->lookup("x");
  EXPECT_NE(result, nullptr);
  EXPECT_EQ(result, val);
  EXPECT_DOUBLE_EQ(result->as_number(), kTestNumber);
}

TEST_F(EnvironmentTest, LookupNonexistent) {
  auto result = env->lookup("nonexistent");
  EXPECT_EQ(result, nullptr);
}

TEST_F(EnvironmentTest, EnvironmentChaining) {
  auto parent_env = std::make_shared<Environment>();
  auto child_env = std::make_shared<Environment>(parent_env);

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
  auto parent_val = make_number(100);
  auto child_val = make_string("hello");

  parent_env->define("parent_var", parent_val);
  child_env->define("child_var", child_val);

  EXPECT_EQ(child_env->lookup("child_var"), child_val);
  EXPECT_EQ(child_env->lookup("parent_var"), parent_val);
  EXPECT_EQ(parent_env->lookup("child_var"), nullptr);
  EXPECT_EQ(parent_env->lookup("parent_var"), parent_val);
}

TEST_F(EnvironmentTest, ShadowingInChildEnvironment) {
  auto parent_env = std::make_shared<Environment>();
  auto child_env = std::make_shared<Environment>(parent_env);

  // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
  auto parent_val = make_number(100);
  auto child_val = make_number(200);
  // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

  parent_env->define("var", parent_val);
  child_env->define("var", child_val);

  EXPECT_EQ(child_env->lookup("var"), child_val);
  EXPECT_EQ(parent_env->lookup("var"), parent_val);
}

TEST_F(EnvironmentTest, ExtendEnvironment) {
  auto val = make_string("test");
  env->define("existing", val);

  auto extended_env = env->extend();
  EXPECT_NE(extended_env, nullptr);

  EXPECT_EQ(extended_env->lookup("existing"), val);

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
  auto new_val = make_number(123);
  extended_env->define("new_var", new_val);

  EXPECT_EQ(extended_env->lookup("new_var"), new_val);
  EXPECT_EQ(env->lookup("new_var"), nullptr);
}

}  // namespace lisp
