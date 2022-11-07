#ifndef PAGE_INCLUDED
#define PAGE_INCLUDED

#include "ConfigMap.hpp"
#include <filesystem>
#include <string>

namespace cppaper {
  struct Page {
    std::string title;
    std::string html;
    std::filesystem::path path;
    ConfigMap config;
  };
}

#endif //PAGE_INCLUDED
