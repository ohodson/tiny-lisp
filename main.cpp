#include <fstream>
#include <iostream>
#include <string>

#include "repl.hpp"

void print_usage(const char* program_name) {
  std::cout << "Usage: " << program_name << " [file]\n";
  std::cout << "  If no file is provided, starts interactive REPL mode.\n";
  std::cout << "  If file is provided, evaluates the file and exits.\n";
}

int main(int argc, char* argv[]) {
  try {
    lisp::REPL repl;

    if (argc == 1) {
      // Interactive mode
      repl.run();
    } else if (argc == 2) {
      // File mode
      std::string filename = argv[1];

      if (filename == "--help" || filename == "-h") {
        print_usage(argv[0]);
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
          std::cout << result->to_string() << std::endl;
        }
      }
    } else {
      print_usage(argv[0]);
      return 1;
    }

  } catch (const std::exception& e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}