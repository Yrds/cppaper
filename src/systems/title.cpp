#include "systems/title.hpp"

#include "components/FileComponent.hpp"
#include "components/PathComponent.hpp"
#include "components/TitleComponent.hpp"
#include "components/IndexFileComponent.hpp"

#include <algorithm>

namespace cppaper {
void titleSystem(entt::registry &registry) {
  //TODO when MarkdownComponent, try to get title by  getting <h1> tag inner text
  auto fileView = registry.view<OriginPathComponent, const FileComponent>();

  fileView.each([&registry](const auto fileEntity, const auto &originPath) {
    auto fileStem = originPath.path.stem().string();

    std::replace_if(
        std::begin(fileStem), std::end(fileStem),
        [](std::string::value_type str) { return str == '-'; }, ' ');

    registry.emplace<TitleComponent>(fileEntity, fileStem);
  });

  auto indexFileView = registry.view<const IndexFileComponent>();

  indexFileView.each([&registry](const auto indexFileEntity, const auto indexFile) {

    auto fileStem = indexFile.indexFilePath.stem().string();

    std::replace_if(
        std::begin(fileStem), std::end(fileStem),
        [](std::string::value_type str) { return str == '-'; }, ' ');

      registry.emplace<TitleComponent>(indexFileEntity, fileStem);
      });
}
} // namespace cppaper
