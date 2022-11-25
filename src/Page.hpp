#ifndef PAGE_INCLUDED
#define PAGE_INCLUDED

#include "ConfigMap.hpp"
#include <filesystem>
#include <string>

namespace cppaper {
struct Page {
  std::string title;
  std::filesystem::path path;
  ConfigMap config;
};
} // namespace cppaper

#endif // PAGE_INCLUDED
