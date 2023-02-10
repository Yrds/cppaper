#include "systems/template.hpp"

#include "inja/inja.hpp"

#include "components/ChildFileComponent.hpp"
#include "components/Config.hpp"
#include "components/FileComponent.hpp"
#include "components/GeneratedContentComponent.hpp"
#include "components/HtmlComponent.hpp"
#include "components/PageContent.hpp"
#include "components/ParentDirectory.hpp"
#include "components/ParentSite.hpp"
#include "components/PathComponent.hpp"
#include "components/TitleComponent.hpp"
#include "components/RawFileComponent.hpp"
#include "components/IndexFileComponent.hpp"
#include "components/JSONComponent.hpp"

namespace cppaper {

inja::Template getTemplate(entt::registry &registry, const entt::entity entity,
                           inja::Environment &env) {
  if (auto indexFile = registry.try_get<IndexFileComponent>(entity); indexFile) {
    return env.parse_template("./templates/" + indexFile->indexFilePath.string());
  }

  if (registry.any_of<HTMLComponent>(entity)) {
    std::string templatePath =
        registry.get<OriginPathComponent>(entity).path.string();

    return env.parse_template(templatePath);
  }
  if (auto config = registry.try_get<ConfigComponent>(entity);
      config && config->map.contains("template")) {
    return env.parse_template("templates/" + config->map["template"]);
  }

  //std::string templatePath =
  //      registry.get<OriginPathComponent>(entity).path.string();
  return env.parse("{{page.html}}");
  //}
}

void setDefaultEnvironmentVariables(inja::json &data) {
  data["site"]["template_dir"] = std::filesystem::path("./templates/").string();
}

// TODO make this a callback(and add filter to files that user don't want to see(regex? maybe))
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

    relativePath.replace_extension(".html"); // TODO this logics seems strange

    data["directory"]["pages"] +=
        {{"title", registry.get<TitleComponent>(fileEntity).title},
         {"path", relativePath.string()},
         {"id", fileEntity}};
  }
}

static inja::Environment env;

void generateContent(entt::registry &registry, const entt::entity entity,
                            const ParentDirectoryComponent &parentDirectory,
                            const ParentSite &parentSite,
                            const ConfigComponent &config,
                            const TitleComponent &title) {

    if(registry.any_of<RawFileComponent>(entity)) {
    std::cout << "Ignoring raw file" << std::endl; //TODO remove
      return; //TODO Ignore rawFile
    }

    inja::Template templ = getTemplate(registry, entity, env);

    inja::json data;

    setDefaultEnvironmentVariables(data);

    loadDirectoryPages(parentDirectory.entity, registry, data);

    if(auto jsonComponent = registry.try_get<JSONComponent>(entity)) {
      std::cout << "loading json" << std::endl;
      data["json"] = jsonComponent->data;
    }

    for (const auto &[config, value] : config.map) {
      data["page"]["config"][config] = value;
    }

    for (const auto &[config, value] :
         registry.get<ConfigComponent>(parentDirectory.entity).map) {
      data["directory"]["config"][config] = value;
    }

    for (const auto &[config, value] :
         registry.get<ConfigComponent>(parentSite.entity).map) {
      data["site"]["config"][config] = value;
    }

    if (auto pageContent = registry.try_get<PageContentComponent>(entity);
        pageContent) {
      data["page"]["html"] = pageContent->content;
    }

    data["page"]["title"] = title.title;

    registry.emplace<GeneratedContentComponent>(entity,
                                                env.render(templ, data));
}

void templateSystem(entt::registry &registry) {
  auto view = registry.view<const FileComponent, const ParentDirectoryComponent,
                            const ParentSite, const ConfigComponent,
                            const TitleComponent>();

  //FIXME IndexFileComponent are not being included in this view
  view.each([&registry](const auto entity, const auto &parentDirectory,
                        const auto &parentSite, const auto &config,
                        const auto &title) {
    generateContent(registry, entity, parentDirectory, parentSite, config,
                    title);
  });

  //auto indexFileView =
  //    registry.view<const IndexFileComponent, const ParentDirectoryComponent,
  //                  const ParentSite, const ConfigComponent,
  //                  const TitleComponent>();

  //indexFileView.each([&registry](const auto entity, const auto &indexFile,
  //                               const auto &parentDirectory,
  //                               const auto &parentSite, const auto &config,
  //                               const auto &title) {
  //  generateContent(registry, entity, parentDirectory, parentSite, config,
  //                  title);
  //});

  // TODO separate entities that don't have templates and those who have.
}
} // namespace cppaper
