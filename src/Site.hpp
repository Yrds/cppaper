#ifndef SITE_INCLUDED
#define SITE_INCLUDED

#include "ConfigMap.hpp"
#include "Directory.hpp"

namespace cppaper {
struct Site {
  ConfigMap config;
  // std::vector<Directory> directories;
  Directory directory;
};
} // namespace cppaper
#endif // SITE_INCLUDED
