#include "template/functions/getPagesFrom.hpp"

#include "components/ChildFileComponent.hpp"
#include "components/PathComponent.hpp"
#include "components/SystemConfigComponent.hpp"
#include "components/TitleComponent.hpp"

namespace cppaper {
void getPagesFrom(entt::registry &registry, inja::Environment& env) {

  env.add_callback("getPagesFrom", 1, [&registry](inja::Arguments args) {
    auto systemEntity = registry.view<SystemConfigComponent>().front();
    auto systemConfig = registry.get<SystemConfigComponent>(systemEntity);

    auto relativePath = args.at(0)->get<std::string>();

    inja::json data = inja::json::array();

    if (systemConfig.directoriesMap.contains(relativePath)) {

      std::cout << "relative path found" << std::endl;

      auto directoryEntity = systemConfig.directoriesMap[relativePath];

      auto dirChildren = registry.get<ChildFileComponent>(directoryEntity);

      for (const auto fileEntity : dirChildren.children) {
        auto &originPath = registry.get<OriginPathComponent>(fileEntity);

        auto relativePath = std::filesystem::path(
            std::filesystem::relative(originPath.path,
                                      std::filesystem::path("pages"))
                .string());

        relativePath.replace_extension(
            ".html"); // TODO this logics seems strange

        data += {{"title", registry.get<TitleComponent>(fileEntity).title},
                 {"path", relativePath.string()},
                 {"id", fileEntity}};
      }

      // TODO retrieve pages of a directory given the RELATIVEPATH
    }

    return data;
  });
}
} // namespace cppaper
