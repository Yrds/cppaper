#include "components/Config.hpp"

#include <map>
#include <string>
#include <iostream> // Remove


#include "components/TitleComponent.hpp" //remove
#include "systems/tag.hpp"
#include "string_utils.hpp"

namespace cppaper {

//TODO make a getPagesByTag('string') to be accessed by template
void createTagIndex(entt::registry &registry) {
  auto configView = registry.view<const ConfigComponent>();

  configView.each([&registry](auto entity, const auto &config) {

    if (auto config = registry.try_get<ConfigComponent>(entity);
        config && config->map.contains("tags")) {

      const std::vector<std::string>& tags = split(config->map["tags"], ',');

      for(const auto& tag: tags) {
        if(tagIndex.contains(tag)) {
          std::cout << "Pushing" << std::endl;
          tagIndex[tag].push_back(entity);
        } else {
          tagIndex[tag] = {entity};
        }
      }

    }

  });
}

std::vector<entt::entity> getPagesByTag(std::string tag) {
  if(tagIndex.contains(tag)) {
    return tagIndex[tag];
  } else {
    return std::vector<entt::entity>();
  }
}
} // namespace cppaper
