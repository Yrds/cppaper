#include "systems/directoriesMap.hpp"

#include "components/DirectoryComponent.hpp"
#include "components/PathComponent.hpp"
#include "components/SystemConfigComponent.hpp"

namespace cppaper {
void directoriesMapSystem(entt::registry& registry) {
  auto directoriesView =
      registry.view<const OriginPathComponent, const DirectoryComponent>();

  auto systemEntity = registry.view<SystemConfigComponent>().front();
  SystemConfigComponent systemConfig =
      registry.get<SystemConfigComponent>(systemEntity);

  directoriesView.each(
      [&systemConfig](const auto entity, const auto pathComponent) {
        systemConfig.directoriesMap[pathComponent.path] = entity;
      });
}
} // namespace cppaper
