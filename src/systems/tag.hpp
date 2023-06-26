#ifndef CPPAPER_TAG_SYSTEM_INCLUDED
#define CPPAPER_TAG_SYSTEM_INCLUDED

#include "entt/entt.hpp"
#include <vector>
#include <string>

namespace cppaper {
static std::map<std::string, std::vector<entt::entity>> tagIndex;
void createTagIndex(entt::registry &registry);
std::vector<entt::entity> getPagesByTag(std::string tag);
}

#endif //Tag_INCLUDED
