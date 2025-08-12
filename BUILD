load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "evaluator_lib",
    srcs = ["evaluator.cpp"],
    hdrs = ["evaluator.hpp"],
    deps = [
        ":value_lib",
    ]
)

cc_library(
    name = "parser_lib",
    srcs = ["parser.cpp"],
    hdrs = ["parser.hpp"],
    deps = [
        ":tokenizer_lib",
        ":value_lib",
    ]
)

cc_library(
    name = "repl_lib",
    srcs = ["repl.cpp"],
    hdrs = ["repl.hpp"],
    deps = [
        ":evaluator_lib",
        ":parser_lib",
        ":tokenizer_lib",
        ":value_lib",
    ]
)

cc_library(
    name = "tokenizer_lib",
    srcs = ["tokenizer.cpp"],
    hdrs = ["tokenizer.hpp"],
)

cc_library(
    name = "value_lib",
    srcs = ["value.cpp"],
    hdrs = ["value.hpp"],
)

cc_binary(
    name = "tiny_lisp",
    srcs = [
        "main.cpp",
    ],
    copts = [
        "-std=c++17",
        "-Wall",
        "-Wextra",
    ],
    deps = [
        ":evaluator_lib",
        ":parser_lib",
        ":repl_lib",
        ":tokenizer_lib",
        ":value_lib",
    ],
    data = [ "//examples:factorial.lisp" ]
)
