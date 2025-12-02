#ifndef CPPAPER_ERRORS_INCLUDED
#define CPPAPER_ERRORS_INCLUDED

#include <cstdint>

namespace cppaper {

enum class ErrorType: uint8_t {
  // This will be returned on main function so starts in 135 so no confusion between errno
  kCppaperLuaNotFound = 135,
  kNinjaBuildFileCreationFailed,
  kCppaperIsNotRegularFile,
  kUnknownError,
  kUnknownCommand,
};

auto print_error(ErrorType error, bool break_line = false) -> void;

}
#endif //CPPAPER_ERRORS_INCLUDED
