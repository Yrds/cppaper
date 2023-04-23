#include<iostream>

#include "systems/directoriesMap.hpp"

#include "components/DirectoryComponent.hpp"
#include "components/RelativePath.hpp"
#include "components/SystemConfigComponent.hpp"



namespace cppaper {
void directoriesMapSystem(entt::registry& registry) {
  auto directoriesView =
      registry.view<const RelativePathComponent, const DirectoryComponent>();

  auto systemEntity = registry.view<SystemConfigComponent>().front();
  SystemConfigComponent& systemConfig =
      registry.get<SystemConfigComponent>(systemEntity);

  std::cout << "directories map:" << std::endl;

  directoriesView.each(
      [&systemConfig](const auto entity, const auto pathComponent) {
        std::cout << static_cast<int>(entity) << ": " << pathComponent.path.string() << std::endl;
        systemConfig.directoriesMap[pathComponent.path.string()] = entity;
      });
}
} // namespace cppaper
