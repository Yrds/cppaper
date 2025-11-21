#ifndef CPPAPER_ERRORS_INCLUDED
#define CPPAPER_ERRORS_INCLUDED

#include <cstdint>
#include <string>

namespace cppaper {

enum class ErrorType: uint8_t {
  kCppaperLuaNotFound = 1,
  kNinjaBuildFileCreationFailed,
  kCppaperIsNotRegularFile,
  kUnknownError,
};

template<typename T>
struct ErrorMessage {
  ErrorType type;
  std::string message;
};

}
#endif //CPPAPER_ERRORS_INCLUDED
