#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Site.hpp"
#include "cmark-gfm.h"

#include "components/ParentDirectory.hpp"
#include "components/ChildFileComponent.hpp"
#include "components/Config.hpp"
#include "components/DirectoryComponent.hpp"
#include "components/FileComponent.hpp"
#include "components/GeneratedContentComponent.hpp"
#include "components/HtmlComponent.hpp"
#include "components/IndexFileComponent.hpp"
#include "components/JSONComponent.hpp"
#include "components/MarkdownComponent.hpp"
#include "components/PageContent.hpp"
#include "components/ParentSite.hpp"
#include "components/PathComponent.hpp"
#include "components/RawFileComponent.hpp"
#include "components/Site.hpp"
#include "components/SystemConfigComponent.hpp"

#include "systems/config.hpp"
#include "systems/directoriesMap.hpp"
#include "systems/index.hpp"
#include "systems/json.hpp"
#include "systems/relativePath.hpp"
#include "systems/template.hpp"
#include "systems/title.hpp"

#include "entt/entt.hpp"

// NOTE Always before a release:
// const correctness
// format code

namespace cppaper {

void getSite(entt::registry &registry) {
  const std::filesystem::path sitePath("./");

  const auto site = registry.create();

  registry.emplace<SiteComponent>(site);
  registry.emplace<OriginPathComponent>(site, sitePath);
}

void loadSiteDirectories(entt::registry &registry) {
  const auto view = registry.view<OriginPathComponent, SiteComponent>();

  view.each([&registry](const auto siteEntity, const auto &path) {
    std::filesystem::path directoryPath{"pages"};

    // TODO optmize logic of this code

    {
      const auto directoryEntity = registry.create();

      registry.emplace<DirectoryComponent>(directoryEntity);

      registry.emplace<ParentSite>(directoryEntity, siteEntity);
      registry.emplace<ChildFileComponent>(directoryEntity);

      registry.emplace<OriginPathComponent>(directoryEntity, directoryPath);
    }

    std::queue<std::filesystem::path> directories{{"pages"}};

    while (!directories.empty()) {
      auto directory = directories.front();
      for (auto const &dirEntry :
           std::filesystem::directory_iterator{directory}) {

        if (std::filesystem::is_directory(dirEntry)) {
          const auto directoryEntity = registry.create();

          registry.emplace<DirectoryComponent>(directoryEntity);
          registry.emplace<ChildFileComponent>(directoryEntity);
          registry.emplace<ParentSite>(directoryEntity, siteEntity);

          registry.emplace<OriginPathComponent>(directoryEntity,
                                                dirEntry.path());

          directories.push(dirEntry.path());
        }
      }

      directories.pop();
    }
  });
}

void loadSiteFiles(entt::registry &registry) {
  auto directoriesView =
      registry.view<const OriginPathComponent, const DirectoryComponent,
                    ChildFileComponent>();

  // NOTE Feature: Allow multiple sites and get ParentSite from dirEntity
  auto siteEntity = registry.view<const SiteComponent>().front();

  directoriesView.each([&registry, &siteEntity](const auto dirEntity,
                                                const auto &originPath,
                                                auto &children) {
    for (auto const &dirEntry :
         std::filesystem::directory_iterator{originPath.path}) {

      if (!std::filesystem::is_regular_file(dirEntry) ||
          dirEntry.path().filename() == "config" ||
          dirEntry.path().filename().string().ends_with(".config")) {
        continue;
      }

      const auto fileEntity = registry.create();

      registry.emplace<FileComponent>(fileEntity);
      registry.emplace<ParentSite>(fileEntity, siteEntity);
      registry.emplace<ParentDirectoryComponent>(fileEntity, dirEntity);
      children.children.push_back(fileEntity);

      auto const pathExtension = dirEntry.path().extension().string();

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
        registry.emplace<OriginPathComponent>(fileEntity, dirEntry.path());
        continue;
      }

      registry.emplace<OriginPathComponent>(fileEntity, dirEntry.path());
      // TODO replace or add relativePathComponent?

      registry.emplace<PageContentComponent>(
          fileEntity); // TODO not every PageContent(?)
    }
  });
}

void generateContent(entt::registry &registry) {
  const auto markdownView =
      registry.view<const OriginPathComponent, PageContentComponent,
                    const MarkdownComponent, const FileComponent,
                    const ConfigComponent>();

  markdownView.each(
      [](const auto &originPath, auto &pageContent, auto &config) {
        std::ifstream mdFile(originPath.path, std::ios::binary);

        if (mdFile.is_open()) {
          std::stringstream ss;

          ss << mdFile.rdbuf();

          auto mdContent = ss.str();

          int cmark_options = CMARK_OPT_DEFAULT;

          if (auto markdownUnsafe = config.map.find("markdown_unsafe");
              markdownUnsafe != config.map.end() &&
              markdownUnsafe->second == "true") {
            cmark_options |= CMARK_OPT_UNSAFE;
          }

          // TODO Include cmark extensions
          pageContent.content = std::string{cmark_markdown_to_html(
              mdContent.c_str(), mdContent.size(), cmark_options)};
        }
      });
}

void clearDirectory(std::filesystem::path directory) {
  if (std::filesystem::is_directory(directory)) {
    for (auto const &dirEntry :
         std::filesystem::directory_iterator{directory}) {
      if (dirEntry.path().filename() != "assets" &&
          dirEntry.path().filename() !=
              ".git") { // TODO make a `ignore` key on config files to no delete
                        // during output
        std::filesystem::remove_all(dirEntry.path());
      }
    }
  }
}

void outputContent(entt::registry &registry) {

  const auto directoryView =
      registry.view<const OriginPathComponent, const DirectoryComponent>();

  const std::filesystem::path pagesPath{"pages"};

  const auto systemEntity = registry.view<SystemConfigComponent>().front();
  const std::filesystem::path publicDirectory =
      registry.get<SystemConfigComponent>(systemEntity).publicDirectory;

  clearDirectory(publicDirectory);

  std::filesystem::create_directory(publicDirectory);

  directoryView.each([&pagesPath, &publicDirectory](const auto &originPath) {
    auto destinationPath = std::filesystem::path(
        publicDirectory.string() +
        std::filesystem::relative(originPath.path, pagesPath).string());

    std::filesystem::create_directory(destinationPath);
  });

  const auto contentView =
      registry.view<const GeneratedContentComponent, const OriginPathComponent,
                    FileComponent>();

  const auto rawFileView =
      registry.view<const OriginPathComponent, const RawFileComponent>();

  const auto indexFileView =
      registry.view<const ParentDirectoryComponent, const IndexFileComponent,
                    const GeneratedContentComponent>();

  const auto size = contentView.size_hint() + rawFileView.size_hint() +
                    indexFileView.size_hint();

  std::cout << "writing " << size << " files" << std::endl;

  contentView.each([&pagesPath, &publicDirectory](const auto &generatedContent,
                                                  const auto &originPath) {
    auto destinationPath = std::filesystem::path(
        publicDirectory.string() +
        std::filesystem::relative(originPath.path, pagesPath).string());

    destinationPath.replace_extension(".html");

    std::ofstream outputPageFile(destinationPath);

    if (outputPageFile.is_open()) {
      std::stringstream ss;

      ss << generatedContent.content;

      outputPageFile << ss.rdbuf();
    } else {
      // throw "Failed to write file " + destinationPath.string();
      throw std::invalid_argument("Failed to write file: " +
                                  destinationPath.string());
    }
  });

  // NOTE I wonder if it's better just to ignore files in public directory that
  // doesn't have extension instead of just copying "raw files"
  rawFileView.each([&pagesPath, &publicDirectory](const auto &originPath) {
    auto destinationPath = std::filesystem::path(
        publicDirectory.string() +
        std::filesystem::relative(originPath.path, pagesPath).string());

    std::filesystem::copy(originPath.path, destinationPath);
  });

  indexFileView.each([&registry, &pagesPath, &publicDirectory](
                         const auto &parentDirectory, const auto &indexFile,
                         const auto &generatedContent) {
    auto destinationPath = std::filesystem::path(
        publicDirectory.string() +
        std::filesystem::relative(
            registry.get<OriginPathComponent>(parentDirectory.entity).path,
            pagesPath)
            .string());

    std::ofstream outputPageFile(destinationPath.string() + "/index.html");

    if (outputPageFile.is_open()) {
      std::stringstream ss;

      ss << generatedContent.content;

      outputPageFile << ss.rdbuf();
    } else {
      // throw "Failed to write file " + destinationPath.string();
      throw std::invalid_argument("Failed to write file: " +
                                  destinationPath.string());
    }
  });
}

} // namespace cppaper

void cmdlineParse(std::string arg, std::function<void(std::string)> callback,
                  int argc, char *argv[]) {
  for (auto argi = 0; argi < argc; argi++) {
    if (std::string(argv[argi]) == arg && (argi + 1 <= argc)) {
      callback(std::string(argv[argi + 1]));
    }
  }
}

// TODO config system?
void setSystem(entt::registry &registry) {
  using namespace cppaper;

  const auto systemEntity = registry.create();
  registry.emplace<SystemConfigComponent>(systemEntity);
}

int main(int argc, char *argv[], char *envp[]) try {
  using namespace cppaper;

  std::cout << "[DEPRECATION] directory.pages is deprecated in favor of getPagesFrom() template function and will be removed in future versions" << std::endl;

  entt::registry registry;

  setSystem(registry);

  cmdlineParse(
      "-C", [](std::string value) { std::filesystem::current_path(value); },
      argc, argv);

  cmdlineParse(
      "-O",
      [&registry](std::string value) {
        const auto systemEntity =
            registry.view<SystemConfigComponent>().front();

        auto &systemConfig = registry.get<SystemConfigComponent>(systemEntity);

        systemConfig.publicDirectory = std::filesystem::path{value + "/"};
      },
      argc, argv);
  // TODO this is only temporary, I will make more robust

  // TODO Refactoring name of system functions to System Acronym and move to
  // properly directories "systems"
  getSite(registry);

  loadSiteDirectories(registry);

  loadSiteFiles(registry);

  relativePathSystem(registry);

  directoriesMapSystem(registry);

  jsonSystem(registry);

  configSystem(registry);

  indexSystem(registry);

  titleSystem(registry);

  // TODO make a site map and add it to template config(only for
  // GeneratedContentComponent)

  generateContent(registry);

  templateSystem(registry);

  // TODO markdown output html on output content when there is no template
  outputContent(registry);

  std::cout << "Done!" << std::endl;

  return 0;
} catch (const std::exception &ex) {
  std::cerr << ex.what() << std::endl;
  return 1;
} catch (...) {
  std::cerr << "An unknown error ocurred" << std::endl;
  return 255;
}
