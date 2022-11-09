#ifndef CPPAPER_CONFIG_HPP_INCLUDED
#define CPPAPER_CONFIG_HPP_INCLUDED

#include "ConfigMap.hpp"
#include <filesystem>

namespace cppaper {
  ConfigMap getConfig(std::filesystem::path &directory);
}

#endif //CPPAPER_CONFIG_HPP_INCLUDED
