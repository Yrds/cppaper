#include "string_utils.hpp"

namespace cppaper {
auto split(std::string& string, const char delimiter) -> std::vector<std::string> {
  std::vector<std::string> tokens;

  std::string next_word;

  for(auto &chr: string) {
    if(chr == delimiter) {
      tokens.push_back(next_word);
      next_word = "";
    } else {
      next_word += chr;
    }
  }

  if(next_word.size() != 0U) {
    tokens.push_back(next_word);
  }

  return tokens;
}
} // namespace cppaper

