#include "systems/title.hpp"

#include "components/FileComponent.hpp"
#include "components/PathComponent.hpp"
#include "components/TitleComponent.hpp"

#include <algorithm>

namespace cppaper {
void titleSystem(entt::registry &registry) {
  auto fileView = registry.view<OriginPathComponent, const FileComponent>();

  fileView.each([&registry](const auto fileEntity, const auto &originPath) {
    auto fileStem = originPath.path.stem().string();

    std::replace_if(
        std::begin(fileStem), std::end(fileStem),
        [](std::string::value_type str) { return str == '-'; }, ' ');

    registry.emplace<TitleComponent>(fileEntity, fileStem);
  });
}
} // namespace cppaper
