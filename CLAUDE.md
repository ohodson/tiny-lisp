# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

This project uses Bazel as its build system with C++17 and LLVM toolchain.

```bash
# Build the interpreter
bazel build :tiny_lisp

# Run in interactive REPL mode  
bazel run :tiny_lisp

# Run a LISP file
bazel run :tiny_lisp -- examples/factorial.lisp

# Generate compile_commands.json for IDE support
bazel run @hedron_compile_commands//:refresh_all
```

## Code Quality Tools

The project uses clang-format and clang-tidy for code quality:

- **Code formatting**: Uses Google style (`.clang-format`)
- **Static analysis**: Comprehensive clang-tidy checks (`.clang-tidy`) including abseil, boost, bugprone, cert, clang-analyzer, concurrency, cppcoreguidelines, google, hicpp, misc, performance, portability, and readability checks

## Architecture Overview

The interpreter follows a clean modular design with clear separation of concerns:

### Core Components

1. **Value System** (`value.hpp/cpp`)
   - Central data structures for all LISP values
   - Uses `std::variant` to represent different value types (NIL, NUMBER, STRING, SYMBOL, CONS, BUILTIN, LAMBDA)
   - Environment class manages variable bindings and scoping
   - Uses shared_ptr for memory management

2. **Tokenizer** (`tokenizer.hpp/cpp`)
   - Lexical analysis converting text into tokens
   - Handles numbers, strings, symbols, parentheses, quotes
   - Foundation for the parsing pipeline

3. **Parser** (`parser.hpp/cpp`)
   - Transforms tokens into Abstract Syntax Tree (AST)
   - Builds Value objects representing parsed expressions
   - Depends on tokenizer and value system

4. **Evaluator** (`evaluator.hpp/cpp`)  
   - Core evaluation engine for LISP expressions
   - Contains all built-in functions (arithmetic, list ops, predicates)
   - Handles special forms (if, quote, lambda, define)
   - Manages environment and variable resolution
   - Depends on value system

5. **REPL** (`repl.hpp/cpp`)
   - Read-Eval-Print loop for interactive use
   - File processing for batch execution
   - Integrates tokenizer, parser, and evaluator
   - Top-level orchestration component

6. **Main** (`main.cpp`)
   - Command-line interface and entry point
   - Handles both interactive and file modes
   - Basic argument parsing and error handling

### Dependencies Flow

The dependency graph flows cleanly with minimal coupling:
- `main.cpp` → `repl` → `evaluator`, `parser`, `tokenizer` → `value`
- Each component has clear interfaces and single responsibilities
- Build system reflects these dependencies with separate libraries

### Key Design Patterns

- **Visitor Pattern**: Value types use std::variant with type-safe access
- **Environment Chain**: Lexical scoping through linked environments
- **Smart Pointers**: Memory safety with shared_ptr for shared ownership
- **Exception Safety**: Custom exception types (EvalError, ParseError) for error handling

## Language Features

The interpreter implements a minimal but complete LISP dialect supporting:
- Numeric and string literals, symbols, lists
- Arithmetic operations with variadic arguments
- List manipulation (car, cdr, cons, list)
- Conditional evaluation (if)
- Function definition (lambda) and variable binding (define)  
- Lexical scoping and closures
- Quote mechanism for preventing evaluation

## Build System Details

Uses Bazel with:
- C++17 standard with strict warnings (-Wall, -Wextra)
- LLVM 19.1.0 toolchain 
- Modular library structure matching the architecture
- Hedron compile commands for IDE integration
- Example files included as build data