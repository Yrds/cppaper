#include "systems/template.hpp"

#include "inja/inja.hpp"

#include "components/FileComponent.hpp"
#include "components/ChildFileComponent.hpp"
#include "components/GeneratedContentComponent.hpp"
#include "components/PageContent.hpp"
#include "components/HtmlComponent.hpp"
#include "components/PathComponent.hpp"
#include "components/ParentDirectory.hpp"
#include "components/ParentSite.hpp"
#include "components/Config.hpp"
#include "components/TitleComponent.hpp"

namespace cppaper {

inja::Template getTemplate(entt::registry &registry, entt::entity entity, inja::Environment& env) {
  if(registry.any_of<HTMLComponent>(entity)){
    std::string templatePath = registry.get<OriginPathComponent>(entity).path.string();

    return env.parse_template(templatePath);
  }
  if(auto config = registry.try_get<ConfigComponent>(entity); config && config->map.contains("template")) {
    return env.parse_template("templates/"+config->map["template"]);
  }

  std::string templatePath = registry.get<OriginPathComponent>(entity).path.string();

  return env.parse_template(templatePath);
  //}
}

//TODO make this a callback
void loadDirectoryPages(entt::entity directoryEntity, entt::registry &registry,
                        inja::json &data) {
  auto dirChildren = registry.get<ChildFileComponent>(directoryEntity);

  data["directory"]["pages"] = {};

  for (const auto fileEntity : dirChildren.children) {
    auto &originPath = registry.get<OriginPathComponent>(fileEntity);

    auto relativePath = std::filesystem::path(
        std::filesystem::relative(originPath.path,
                                  std::filesystem::path("pages"))
            .string());

    relativePath.replace_extension(".html"); //TODO this logics seems strange

    data["directory"]["pages"] += { 
      { "title", registry.get<TitleComponent>(fileEntity).title },
      { "path",  relativePath.string() }
    };
  }
}

static inja::Environment env;

void templateSystem(entt::registry &registry) {
  auto view = registry.view<const FileComponent, const ParentDirectoryComponent, const ParentSite, const ConfigComponent, const TitleComponent>();


  view.each([&registry](const auto entity, const auto& parentDirectory, const auto& parentSite, const auto& config, const auto& title) {

    inja::Template templ = getTemplate(registry, entity, env);

    inja::json data;

    loadDirectoryPages(parentDirectory.entity, registry, data);

    for (const auto &[config, value] : config.map) {
      data["page"]["config"][config] = value;
    }

    for (const auto &[config, value] : registry.get<ConfigComponent>(parentDirectory.entity).map) {
      data["directory"]["config"][config] = value;
    }

    for (const auto &[config, value] : registry.get<ConfigComponent>(parentSite.entity).map) {
      data["site"]["config"][config] = value;
    }

    if(auto pageContent = registry.try_get<PageContentComponent>(entity); pageContent){
      data["page"]["html"] = pageContent->content;
    }

    data["page"]["title"] = title.title;

    registry.emplace<GeneratedContentComponent>(entity, env.render(templ, data));

  });

  // TODO separate entities that don't have templates and those who have.
}
} // namespace cppaper
