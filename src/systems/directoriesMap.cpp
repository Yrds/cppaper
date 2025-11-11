#include<iostream>

#include "systems/directoriesMap.hpp"

#include "components/DirectoryComponent.hpp"
#include "components/RelativePath.hpp"
#include "components/SystemConfigComponent.hpp"

namespace cppaper {
void directoriesMapSystem(entt::registry& registry) {
  auto directories_view =
      registry.view<const RelativePathComponent, const DirectoryComponent>();

  auto system_entity = registry.view<SystemConfigComponent>().front();
  SystemConfigComponent& system_config =
      registry.get<SystemConfigComponent>(system_entity);

  std::cout << "directories map:" << '\n';

  directories_view.each(
      [&system_config](const auto entity, const auto pathComponent) {
        std::cout << static_cast<int>(entity) << ": " << pathComponent.path.string() << std::endl;
        system_config.directoriesMap[pathComponent.path.string()] = entity;
      });
}
} // namespace cppaper
