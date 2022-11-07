#ifndef DIRECTORY_INCLUDED
#define DIRECTORY_INCLUDED

#include <vector>
#include <filesystem>
#include "ConfigMap.hpp"
#include "Page.hpp"

namespace cppaper {
  struct Directory {
    std::filesystem::path path;
    ConfigMap config;
    std::vector<Page> pages;
  };
}
#endif //DIRECTORY_INCLUDED
