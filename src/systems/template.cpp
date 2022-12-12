#include "systems/template.hpp"

#include "inja/inja.hpp"

#include "components/FileComponent.hpp"
#include "components/GeneratedContentComponent.hpp"
#include "components/PageContent.hpp"
#include "components/PathComponent.hpp"

namespace cppaper {

inja::Template getTemplate(entt::registry &registry, entt::entity entity, inja::Environment& env) {
  // if(auto registry::try_get<Config>(entity) = fileConfig; fileConfig) {
  //   return env.parse_template(templateFilePath);
  // } else {
  std::string templatePath = registry.get<OriginPathComponent>(entity).path.string();
  return env.parse_template(templatePath);
  //}
}

static inja::Environment env;

void templateSystem(entt::registry &registry) {
  auto view = registry.view<const PageContentComponent, const FileComponent>();

  view.each([&registry](const auto entity, auto &generatedContent) {

    inja::Template templ = getTemplate(registry, entity, env);

    inja::json data;

    registry.emplace<GeneratedContentComponent>(entity, env.render(templ, data));

  });

  // TODO separate entities that don't have templates and those who have.
}
} // namespace cppaper
