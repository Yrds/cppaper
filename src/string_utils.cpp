#include "string_utils.hpp"

namespace cppaper {
std::vector<std::string> split(std::string& string, const char delimiter) {
  std::vector<std::string> tokens;

  std::string next_word = "";

  for(auto &c: string) {
    if(c == delimiter) {
      tokens.push_back(next_word);
      next_word = "";
    } else {
      next_word += c;
    }
  }

  if(next_word.size()) {
    tokens.push_back(next_word);
  }

  return tokens;
}
} // namespace cppaper

