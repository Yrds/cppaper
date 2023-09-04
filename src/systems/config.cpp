#include "systems/config.hpp"

#include <filesystem>
#include <fstream>

#include "tfstring.h"

#include "components/Config.hpp"
#include "components/DirectoryComponent.hpp"
#include "components/FileComponent.hpp"
#include "components/ParentDirectory.hpp"
#include "components/ParentSite.hpp"
#include "components/PathComponent.hpp"
#include "components/Site.hpp"
#include "components/SystemConfigComponent.hpp"

namespace cppaper {

ConfigMap getConfig(const std::filesystem::path &directory) {
  ConfigMap directoryConfig;

  std::ifstream configFile;

  if (std::filesystem::is_directory(directory)) {
    configFile.open(directory.string() + "/config");
  } else {
    configFile.open(directory.string() + ".config");
  }

  if (configFile.is_open()) {
    std::string line;
    while (std::getline(configFile, line)) {
      auto delimiterPosition = line.find("=");
      if (delimiterPosition != std::string::npos) {
        directoryConfig[line.substr(0, delimiterPosition)] =
            line.substr(delimiterPosition + 1);
      }
    }
  }

  return directoryConfig;
}

void configSystem(entt::registry &registry) {

  auto siteView =
      registry.view<const OriginPathComponent, const SiteComponent>();

  siteView.each([&registry](auto siteEntity, const auto &originPath) {
    registry.emplace<ConfigComponent>(siteEntity, getConfig(originPath.path));

    const auto &systemEntity = registry.view<SystemConfigComponent>().front();

    SystemConfigComponent &systemConfig =
      registry.get<SystemConfigComponent>(systemEntity);

    auto &siteConfig = registry.get<ConfigComponent>(siteEntity);

    if(siteConfig.map.contains("lua_libraries")){
      systemConfig.luaLibraries = tf::string{ siteConfig.map["lua_libraries"] }.split(',');
    }

  });

  auto directoryView =
      registry.view<const ParentSite, const OriginPathComponent,
                    const DirectoryComponent>();

  directoryView.each([&registry](const auto dirEntity, const auto &parentSite,
                                 auto &originPath) {
    auto directoryConfig = getConfig(originPath.path);

    registry.emplace<ConfigComponent>(
        dirEntity, registry.get<ConfigComponent>(parentSite.entity));
    auto &dirConfig = registry.get<ConfigComponent>(dirEntity);

    for (const auto &[key, value] : directoryConfig) {
      dirConfig.map[key] = value;
    }
  });

  auto fileView =
      registry.view<const ParentDirectoryComponent, const OriginPathComponent,
                    const FileComponent>();

  fileView.each([&registry](const auto fileEntity, const auto &parentDirectory,
                            const auto &originPath) {
    auto tempConfig = getConfig(originPath.path);

    registry.emplace<ConfigComponent>(
        fileEntity, registry.get<ConfigComponent>(parentDirectory.entity));

    auto &fileConfig = registry.get<ConfigComponent>(fileEntity);

    for (const auto &[key, value] : tempConfig) {
      fileConfig.map[key] = value;
    }
  });
}
} // namespace cppaper
