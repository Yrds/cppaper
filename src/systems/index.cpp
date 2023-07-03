#include "systems/index.hpp"

#include <stdexcept>

#include "components/Config.hpp"
#include "components/DirectoryComponent.hpp"
#include "components/FileComponent.hpp"
#include "components/HtmlComponent.hpp"
#include "components/IndexFileComponent.hpp"
#include "components/ParentDirectory.hpp"
#include "components/ParentSite.hpp"
#include "components/RelativePath.hpp"
#include "components/PathComponent.hpp"

namespace cppaper {
void indexSystem(entt::registry &registry) {
  auto directoryView = registry.view<const ConfigComponent, const ParentSite,
                                     const DirectoryComponent>();

  const std::filesystem::path pagesPath{"pages"};

  directoryView.each([&registry, &pagesPath](const auto directoryEntity,
                                             const auto &config,
                                             const auto siteEntity) {
    if (auto indexConfig = config.map.find("index");
        indexConfig != config.map.end()) {
      const auto indexFileEntity = registry.create();
      registry.emplace<FileComponent>(indexFileEntity);
      registry.emplace<ParentSite>(indexFileEntity, siteEntity);
      registry.emplace<ParentDirectoryComponent>(indexFileEntity,
                                                 directoryEntity);

      if (auto indexTemplateFile =
              std::filesystem::path("./templates/" + indexConfig->second);
          std::filesystem::exists(indexTemplateFile)) {
        registry.emplace<IndexFileComponent>(indexFileEntity,
                                             indexConfig->second);
      } else {
        throw std::invalid_argument("Index file doesn't exists: " +
                                    indexTemplateFile.string());
      }

      registry.emplace<ConfigComponent>(indexFileEntity, config);
      registry.emplace<RelativePathComponent>(
          indexFileEntity,
          std::filesystem::relative(
              std::filesystem::path(registry.get<OriginPathComponent>(directoryEntity).path.string() + "/index.html"),
              pagesPath)
              .string());
    }
  });
};
} // namespace cppaper
