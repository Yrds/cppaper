#include "systems/sitemap.hpp"

#include "components/Config.hpp"
#include "components/DirectoryComponent.hpp"
#include "components/GeneratedContentComponent.hpp"
#include "components/RelativePath.hpp"
#include "components/FileComponent.hpp"
#include "components/SitemapComponent.hpp"
#include "components/Config.hpp"
#include "components/Site.hpp"
#include "components/NoOutput.hpp"

#include <stdexcept>
#include <string>
#include <iostream>

namespace cppaper {

// NOTE if this Url contains too much attributes, transform into an entt::registry
//struct Urls {
//  std::vector<std::string> loc;
//};

std::string getSiteBaseHref(const ConfigComponent &configComponent) {
  if(configComponent.map.contains("baseHref")) {
    return configComponent.map.at("baseHref");
  } else {
    return "/";
  }
}

void sitemapSystem(entt::registry &registry) {

  std::string sitemapString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
  sitemapString +=
      "<urlset xmlns=\"http://www.sitemaps.org/schemas/sitemap/0.9\">";

  const auto siteEntity = registry.view<const SiteComponent>().front();
  const auto siteConfig = registry.get<const ConfigComponent>(siteEntity);

  const auto siteHref = getSiteBaseHref(siteConfig);

  if(!siteConfig.map.contains("domain")) {
    throw std::invalid_argument("You must have a 'domain' in site config in order to generate a sitemap");
  }

  const auto siteDomain = siteConfig.map.at("domain");

  const auto contentView =
      registry.view<const RelativePathComponent, const FileComponent>(entt::exclude<NoOutputComponent>);

  // NOTE This need site global configuration to work
  contentView.each([&sitemapString, &siteHref,
                    &siteDomain](const auto &relativePathComponent) {
                   std::cout << "generating sitemap.xml to " << relativePathComponent.path << std::endl;
    sitemapString += "<url>";
    sitemapString += "<loc>" + std::string("https://") + siteDomain + siteHref + relativePathComponent.path.string() + +"</loc>";
    sitemapString += "</url>";
  });

  sitemapString += "</urlset>";

  const auto sitemapEntity = registry.create();

  registry.emplace<GeneratedContentComponent>(sitemapEntity, sitemapString);
  registry.emplace<RelativePathComponent>(sitemapEntity, "sitemap.xml");
  registry.emplace<SitemapComponent>(sitemapEntity);
}
} // namespace cppaper
