#include "systems/template.hpp"

#include <iostream>

#include "inja/inja.hpp"

#include "components/FileContentComponent.hpp"
#include "components/ChildFileComponent.hpp"
#include "components/Config.hpp"
#include "components/FileComponent.hpp"
#include "components/GeneratedContentComponent.hpp"
#include "components/HtmlComponent.hpp"
#include "components/IndexFileComponent.hpp"
#include "components/JSONComponent.hpp"
#include "components/PageContent.hpp"
#include "components/ParentDirectory.hpp"
#include "components/ParentSite.hpp"
#include "components/PathComponent.hpp"
#include "components/RawFileComponent.hpp"
#include "components/SystemConfigComponent.hpp"
#include "components/TitleComponent.hpp"

//Template functions
#include "template/functions/getConfigFrom.hpp"
#include "template/functions/getPagesByTag.hpp"
#include "template/functions/getPagesFrom.hpp"
#include "template/functions/getJsonFrom.hpp"
#include "template/functions/getPathFrom.hpp"
#include "template/functions/lua.hpp"
#include "template/functions/getPagesWithConfig.hpp"

//TODO create a function to process generated content as inja templates

namespace cppaper {

//TODO transform this to a component like `TemplateComponent`
inja::Template getTemplate(entt::registry &registry, const entt::entity entity,
                           inja::Environment &env, bool isContent) {

  if (auto indexFile = registry.try_get<IndexFileComponent>(entity); indexFile) {
    return env.parse_template("./templates/" + indexFile->indexFilePath.string());
  }

  if (isContent || registry.any_of<HTMLComponent>(entity)) {
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

static inja::Environment env;

inline void generateContent(entt::registry &registry, const entt::entity entity,
                            const ParentDirectoryComponent &parentDirectory,
                            const ParentSite &parentSite,
                            const ConfigComponent &config,
                            const TitleComponent &title,
                            const bool isContent = false
                            ) {

    if(registry.any_of<RawFileComponent>(entity)) {
    std::cout << "Ignoring raw file" << std::endl; //TODO remove
      return; //TODO Ignore rawFile
    }

    inja::Template templ = getTemplate(registry, entity, env, isContent);

    inja::json data;

    setDefaultEnvironmentVariables(data);

    data["page"]["id"] = static_cast<int>(entity);

    if(auto jsonComponent = registry.try_get<JSONComponent>(entity)) {
      data["page"]["json"] = jsonComponent->data;
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
      if(isContent) {
        if (auto fileContent = registry.try_get<FileContentComponent>(entity);
            fileContent) {
            fileContent->content = env.render(templ, data);
        }
      } else {
        registry.emplace<GeneratedContentComponent>(entity,
                                                    env.render(templ, data));
      }
}

inline void registerCallbacks(entt::registry &registry, inja::Environment &env) {
  getPagesFrom(registry, env);
  getConfigFrom(registry, env);
  getJsonFrom(registry, env);
  getPagesByTagT(registry, env);
  getPathFromT(registry, env);
  getPagesWithConfig(registry, env);
  templ::scripts::luaT(registry, env);
}

void initTemplateEnvironment(entt::registry &registry) {
  env.set_line_statement("%%");

  registerCallbacks(registry, env);
}

void templateSystem(entt::registry &registry) {

  auto view = registry.view<const FileComponent, const ParentDirectoryComponent,
                            const ParentSite, const ConfigComponent,
                            const TitleComponent>();

  view.each([&registry](const auto entity, const auto &parentDirectory,
                        const auto &parentSite, const auto &config,
                        const auto &title) {
    generateContent(registry, entity, parentDirectory, parentSite, config,
                    title);
  });

}


/*
* Process content file by the template engine
* e.g. A markdown file content that wants to be processed
* In other words the file content is the template
*
* NOTE: this can cause problems if you want demonstrate inja or semelhant syntax code into a markdown file
* Because of historical reasons this is enabled by default
* So I added a new option process_as_template where `false` value disables it.
*/
void templateFileContent(entt::registry &registry) {

  auto view = registry.view<const FileComponent, const ParentDirectoryComponent,
                            const ParentSite, const ConfigComponent,
                            const TitleComponent, const FileContentComponent>();

  view.each([&registry](const auto entity, const auto &parentDirectory,
                        const auto &parentSite, const auto &config,
                        const auto &title, const auto& fileContent) {

    if (config.map.contains("process_as_template") && config.map.at("process_as_template") == "false") {
      return;
    }

    generateContent(registry, entity, parentDirectory, parentSite, config,
                    title, true);
  });

}
} // namespace cppaper
