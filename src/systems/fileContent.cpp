#include "components/Config.hpp"
#include "components/FileComponent.hpp"
#include "components/FileContentComponent.hpp"
#include "components/MarkdownComponent.hpp"
#include "components/PathComponent.hpp"
#include "systems/fileContent.hpp"

#include <fstream>
#include <sstream>

#include<iostream>

namespace cppaper {
void readFilesContent(entt::registry &registry) {
  // NOTE for now this only includes MarkdownComponent for template processing
  // But I have plans for the future for all non RawFileComponent

  auto markdownFileView =
      registry.view<const FileComponent, const OriginPathComponent, const MarkdownComponent>();

  markdownFileView.each(
      [&registry](const auto entity, const auto &originPathComponent) {
        std::ifstream mdFile(originPathComponent.path, std::ios::binary);

        if (mdFile.is_open()) {
          std::stringstream ss;

          ss << mdFile.rdbuf();

          registry.emplace<FileContentComponent>(entity, ss.str());
        }
      });
};
} // namespace cppaper
