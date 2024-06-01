#ifndef SYSTEM_CONFIG_COMPONENT_INCLUDED
#define SYSTEM_CONFIG_COMPONENT_INCLUDED

#include <filesystem>
#include <map>
#include <string>

#include "entt/entt.hpp"
#include "sol/sol.hpp"

namespace cppaper {
  struct SystemConfigComponent {
    std::filesystem::path publicDirectory{"public/"};
    std::map<std::filesystem::path, entt::entity> directoriesMap;
    std::vector<std::string> luaLibraries;

    sol::state lua;
  };
}

#endif //SystemConfig_INCLUDED
