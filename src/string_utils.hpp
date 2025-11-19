#ifndef STRING_UTILS_INCLUDED
#define STRING_UTILS_INCLUDED

#include <vector>
#include <string>

namespace cppaper {
auto split(std::string& string, char delimiter) -> std::vector<std::string>;
auto expand(std::string& str, std::vector<std::vector<std::string>>& wildcards) -> std::vector<std::string>;
} // namespace cppaper


#endif //STRING_UTILS_INCLUDED
