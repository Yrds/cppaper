#include "components/Config.hpp"

#include <map>
#include <string>

#include "systems/tag.hpp"
#include "string_utils.hpp"

namespace cppaper {

static inline std::map<std::string, std::vector<entt::entity>> tagIndex;

//TODO make a getPagesByTag('string') to be accessed by template
void createPagesIndex(entt::registry &registry) {
  auto configView = registry.view<const ConfigComponent>();

  configView.each([&registry](auto entity, const auto &config) {
    if (auto config = registry.try_get<ConfigComponent>(entity);
        config && config->map.contains("tags")) {

      const std::vector<std::string>& tags = split(config->map["tags"], ',');

      for(const auto& tag: tags) {
        if(tagIndex.contains(tag)) {
          tagIndex[tag] = {entity};
        } else {
          tagIndex[tag].push_back(entity);
        }
      }

    }
  });
}
} // namespace cppaper
