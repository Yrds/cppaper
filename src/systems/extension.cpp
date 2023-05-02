#include "systems/extension.hpp"

#include "components/PathComponent.hpp"
#include "components/FileComponent.hpp"

#include "components/MarkdownComponent.hpp"
#include "components/RawFileComponent.hpp"
#include "components/JSONComponent.hpp"
#include "components/HtmlComponent.hpp"

namespace cppaper {
void extensionSystem(entt::registry &registry) {

  auto filesView =
      registry.view<const OriginPathComponent, const FileComponent>();

  filesView.each([&registry](const auto fileEntity, const auto pathComponent) {
    auto const pathExtension = pathComponent.path.extension().string();

    // TODO move this to other system or function inside the same system
    if (pathExtension == ".md") {
      registry.emplace<MarkdownComponent>(fileEntity);
    } else if (pathExtension == ".html") {
      registry.emplace<HTMLComponent>(fileEntity);
    } else if (pathExtension == ".json") {
      registry.emplace<JSONComponent>(fileEntity);
    } else {
      // NOTE UGly code
      registry.emplace<RawFileComponent>(fileEntity);
    }
  });
}
} // namespace cppaper
