#include "systems/shortcodes.hpp"

#include <filesystem>
#include "components/PathComponent.hpp"
#include "components/shortcode.hpp"

namespace cppaper {
void shortcodesSystem(entt::registry &registry) {

  for (auto const &dirEntry : std::filesystem::directory_iterator{"shortcodes"}) {
    if (std::filesystem::is_regular_file(dirEntry)) {
      const auto shortcodeFileEntity = registry.create();

      registry.emplace<OriginPathComponent>(shortcodeFileEntity, dirEntry.path());
      registry.emplace<ShortCodeComponent>(shortcodeFileEntity);

    }
  }

  // Find shortcodes on shortcodes directory
  // Create a map to finding shortcodes by name
  // TODO
}
} // namespace cppaper
