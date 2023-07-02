#include "systems/extension.hpp"

#include "components/PathComponent.hpp"
#include "components/FileComponent.hpp"

#include "components/MarkdownComponent.hpp"
#include "components/RawFileComponent.hpp"
#include "components/JSONComponent.hpp"
#include "components/HtmlComponent.hpp"
#include "components/RelativePath.hpp"
#include "components/GeneratedContentComponent.hpp"

namespace cppaper {
void extensionSystem(entt::registry &registry) {

  auto filesView =
      registry.view<const OriginPathComponent, const FileComponent>();

  filesView.each([&registry](const auto fileEntity, const auto &pathComponent) {
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

void setFilesRelativePathExtension(entt::registry &registry) {
  auto generatedContentView = registry.view<RelativePathComponent, const FileComponent>(entt::exclude<RawFileComponent>);

  //NOTE Should this have a OutputExtension to future configurations and customizability(is this word exists?)
  
  generatedContentView.each([](auto &relativePathComponent){
    std::cout << "Converting: " << relativePathComponent.path << " to .html" << std::endl; //TODO REMOVE
    relativePathComponent.path.replace_extension(".html");
  });
}

} // namespace cppaper
