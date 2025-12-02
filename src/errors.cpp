#include "errors.hpp"
#include <iostream>

namespace cppaper {

auto print_error(ErrorType error, bool break_line) -> void {
  using cppaper::ErrorType;

  switch(error) {
    case ErrorType::kCppaperLuaNotFound:
      std::cout << "Lua file not found";
    case ErrorType::kNinjaBuildFileCreationFailed:
      std::cout << "Ninja build file creation failed";
    case ErrorType::kCppaperIsNotRegularFile:
      std::cout << "cppaper.lua is not a regular file";
    case ErrorType::kUnknownCommand:
      std::cout << "No command specified, running default build process...";
    case ErrorType::kUnknownError:
      std::cout << "UnknownError";
  }

  if (break_line) {
    std::cout << '\n';
  }
}

}
