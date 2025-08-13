#include <exception>
#include <fstream>
#include <iostream>
#include <span>
#include <string>

#include "repl.hpp"

namespace {

void print_usage(const std::string& program_name) {
  std::cout << "Usage: " << program_name << " [file]\n";
  std::cout << "  If no file is provided, starts interactive REPL mode.\n";
  std::cout << "  If file is provided, evaluates the file and exits.\n";
}

}  // namespace

int main(int argc, char* argv[]) {
  auto args = std::span(argv, argc);
  try {
    lisp::REPL repl;

    if (argc == 1) {
      // Interactive mode
      repl.run();
    } else if (argc == 2) {
      std::string const program_name = args[0];
      std::string const filename = args[1];

      if (filename == "--help" || filename == "-h") {
        print_usage(program_name);
        return 0;
      }

      std::ifstream file(filename);
      if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'\n";
        return 1;
      }

      std::string content;
      std::string line;
      while (std::getline(file, line)) {
        content += line + "\n";
      }
      file.close();

      if (!content.empty()) {
        auto result = repl.eval_string(content);
        if (result) {
          std::cout << result->to_string() << '\n';
        }
      }
    } else {
      print_usage(args[0]);
      return 1;
    }
  } catch (const std::exception& e) {
    std::cerr << "Fatal error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}