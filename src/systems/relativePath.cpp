#include "systems/relativePath.hpp"
#include "components/PathComponent.hpp"

#include "components/RelativePath.hpp"
#include <string_view>

namespace cppaper {
  void relativePathSystem(entt::registry &registry) {
    auto originPathView = registry.view<const OriginPathComponent>();

    originPathView.each([&registry](const auto entity, const auto originPath){

        constexpr auto pagesPath = std::string_view("pages");

        auto relativePath = std::filesystem::relative(originPath.path, pagesPath).string();

        registry.emplace<RelativePathComponent>(entity, relativePath);

        });
  }
}

