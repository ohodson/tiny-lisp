load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "value_test",
    size = "small",
    srcs = ["value_test.cpp"],
    deps = [
        "//:value_lib",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
    copts = [
        "-std=c++20",
        "-Wall",
        "-Wextra",
    ],
)

cc_test(
    name = "tokenizer_test",
    size = "small",
    srcs = ["tokenizer_test.cpp"],
    deps = [
        "//:tokenizer_lib",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
    copts = [
        "-std=c++20",
        "-Wall",
        "-Wextra",
    ],
)

cc_test(
    name = "parser_test",
    size = "small",
    srcs = ["parser_test.cpp"],
    deps = [
        "//:parser_lib",
        "//:tokenizer_lib",
        "//:value_lib",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
    copts = [
        "-std=c++20",
        "-Wall",
        "-Wextra",
    ],
)

cc_test(
    name = "evaluator_test",
    size = "medium",
    srcs = ["evaluator_test.cpp"],
    deps = [
        "//:evaluator_lib",
        "//:parser_lib",
        "//:tokenizer_lib",
        "//:value_lib",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
    copts = [
        "-std=c++20",
        "-Wall",
        "-Wextra",
    ],
)

cc_test(
    name = "repl_test",
    size = "medium",
    srcs = ["repl_test.cpp"],
    deps = [
        "//:repl_lib",
        "//:evaluator_lib",
        "//:parser_lib",
        "//:tokenizer_lib",
        "//:value_lib",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
    copts = [
        "-std=c++20",
        "-Wall",
        "-Wextra",
    ],
)

# Test suite that runs all tests
test_suite(
    name = "all_tests",
    tests = [
        ":value_test",
        ":tokenizer_test",
        ":parser_test",
        ":evaluator_test",
        ":repl_test",
    ],
    visibility = ["//visibility:public"],
)