#ifndef SYSTEM_CONFIG_COMPONENT_INCLUDED
#define SYSTEM_CONFIG_COMPONENT_INCLUDED

#include "entt/entt.hpp"
#include <filesystem>
#include <map>
#include <string>

namespace cppaper {
  struct SystemConfigComponent {
    std::filesystem::path publicDirectory{"public/"};
    std::map<std::filesystem::path, entt::entity> directoriesMap;
    std::vector<std::string> luaLibraries;
  };
}

#endif //SystemConfig_INCLUDED
