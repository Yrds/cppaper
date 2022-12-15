#ifndef CONFIG_INCLUDED
#define CONFIG_INCLUDED

#include "entt/entt.hpp"
#include <filesystem>
#include <map>
#include <string>

namespace cppaper {
using ConfigMap = std::map<std::string, std::string>;

ConfigMap getConfig(const std::filesystem::path &directory);

void configSystem(entt::registry &registry);
} // namespace cppaper

#endif // Config_INCLUDED
