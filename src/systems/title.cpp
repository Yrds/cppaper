#include "systems/title.hpp"

#include "components/Config.hpp"
#include "components/FileComponent.hpp"
#include "components/PathComponent.hpp"
#include "components/TitleComponent.hpp"
#include "components/IndexFileComponent.hpp"

#include <algorithm>
#include <iostream>

namespace cppaper {

void titleByConfig(entt::registry &registry) {
  auto untitledFileView = registry.view<const FileComponent, const ConfigComponent>(
      entt::exclude<TitleComponent>);

  untitledFileView.each([&registry](auto entity, const auto &configComponent) {
    if (auto config = registry.try_get<ConfigComponent>(entity);
        config && config->map.contains("title")) {
      std::cout << "title " << config->map["title"] << std::endl;
      registry.emplace<TitleComponent>(entity, config->map["title"]);
    }
  });
}

void titleSystem(entt::registry &registry) {
  titleByConfig(registry);
  //TODO when MarkdownComponent, try to get title by  getting <h1> tag inner text
  //
  
  auto fileView = registry.view<OriginPathComponent, const FileComponent>(entt::exclude<TitleComponent>);

  fileView.each([&registry](const auto fileEntity, const auto &originPath) {
    auto fileStem = originPath.path.stem().string();

    std::replace_if(
        std::begin(fileStem), std::end(fileStem),
        [](std::string::value_type str) { return str == '-'; }, ' ');

    registry.emplace<TitleComponent>(fileEntity, fileStem);
  });

  auto indexFileView = registry.view<const IndexFileComponent>(entt::exclude<TitleComponent>);

  indexFileView.each([&registry](const auto indexFileEntity, const auto indexFile) {

    auto fileStem = indexFile.indexFilePath.stem().string();

    std::replace_if(
        std::begin(fileStem), std::end(fileStem),
        [](std::string::value_type str) { return str == '-'; }, ' ');

      registry.emplace<TitleComponent>(indexFileEntity, fileStem);
      });
}
} // namespace cppaper
