#ifndef DIRECTORY_INCLUDED
#define DIRECTORY_INCLUDED

#include "ConfigMap.hpp"
#include "Page.hpp"
#include <filesystem>
#include <vector>

namespace cppaper {
struct Directory {
  std::filesystem::path path;
  ConfigMap config;
  std::vector<Page> pages;
};
} // namespace cppaper
#endif // DIRECTORY_INCLUDED
