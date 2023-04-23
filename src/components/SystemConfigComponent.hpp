#ifndef SYSTEM_CONFIG_COMPONENT_INCLUDED
#define SYSTEM_CONFIG_COMPONENT_INCLUDED

#include "entt/entt.hpp"
#include <filesystem>
#include <map>

namespace cppaper {
  struct SystemConfigComponent { 
    std::filesystem::path publicDirectory{"public/"};
    std::map<std::filesystem::path, entt::entity> directoriesMap;
  };
}

#endif //SystemConfig_INCLUDED
