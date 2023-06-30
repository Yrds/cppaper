#include "systems/sitemap.hpp"

#include "components/GeneratedContentComponent.hpp"
#include "components/RelativePath.hpp"
#include "components/FileComponent.hpp"
#include "components/SitemapComponent.hpp"

#include <string>

namespace cppaper {

// NOTE if this Url contains too much attributes, transform into an entt::registry
//struct Urls {
//  std::vector<std::string> loc;
//};

void sitemapSystem(entt::registry &registry) {
  std::string sitemapString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
  sitemapString += "<urlset xmlns=\"http://www.sitemaps.org/schemas/sitemap/0.9\">";

  const auto contentView =
      registry.view<const RelativePathComponent,
                    const FileComponent>();

  // NOTE This need site global configuration to work
  contentView.each([&sitemapString](const auto& relativePathComponent){
    sitemapString += "<url>";
    sitemapString += "<loc>" +
      std::string("https://") + "domain.com" + "/" + relativePathComponent.path.string() +
      + "</loc>";
    sitemapString += "</url>";

  });

  sitemapString += "</urlset>";

  const auto sitemapEntity = registry.create();

  registry.emplace<GeneratedContentComponent>(sitemapEntity, sitemapString);
  registry.emplace<RelativePathComponent>(sitemapEntity, "sitemap.xml");
  registry.emplace<SitemapComponent>(sitemapEntity);
}
} // namespace cppaper
