#ifndef CONFIG_INCLUDED
#define CONFIG_INCLUDED

#include "entt/entt.hpp"
#include <filesystem>
#include <map>
#include <string>

namespace cppaper {
using ConfigMap = std::map<std::string, std::string>;

static std::map<std::string, std::map<std::string, std::vector<entt::entity>>> configIndex;

ConfigMap getConfig(const std::filesystem::path &directory);

std::vector<entt::entity> getPagesWithConfig(std::string configName);
std::vector<entt::entity> getPagesWithConfig(std::string configName, std::string configValue);

void configSystem(entt::registry &registry);
} // namespace cppaper

#endif // Config_INCLUDED
