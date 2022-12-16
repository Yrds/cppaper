#ifndef SYSTEM_CONFIG_COMPONENT_INCLUDED
#define SYSTEM_CONFIG_COMPONENT_INCLUDED

#include "entt/entt.hpp"
#include <filesystem>

namespace cppaper {
  struct SystemConfigComponent { 
    std::filesystem::path publicDirectory{"public/"};
  };
}

#endif //SystemConfig_INCLUDED
