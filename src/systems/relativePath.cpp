#include "components/PathComponent.hpp"
#include "systems/relativePath.hpp"

#include "components/RelativePath.hpp"
#include <string_view>

namespace cppaper {
void relativePathSystem(entt::registry &registry) {
  auto originPathView = registry.view<const OriginPathComponent>();

  constexpr auto pagesPath = std::string_view("pages");

  originPathView.each(
      [&registry, &pagesPath](const auto entity, const auto originPath) {
        auto relativePath =
            std::filesystem::relative(originPath.path, pagesPath).string();

        registry.emplace<RelativePathComponent>(entity, relativePath);
      });
}
} // namespace cppaper
