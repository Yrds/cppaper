#ifndef CPPAPER_CONFIG_HPP_INCLUDED
#define CPPAPER_CONFIG_HPP_INCLUDED

#include <filesystem>

#include "ConfigMap.hpp"

namespace cppaper {
ConfigMap getConfig(std::filesystem::path &directory);
std::filesystem::path getTemplateDirectory();
} // namespace cppaper

#endif // CPPAPER_CONFIG_HPP_INCLUDED
