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

#include "components/DirectoryComponent.hpp"
#include "components/FileComponent.hpp"
#include "components/GeneratedContentComponent.hpp"
#include "components/MarkdownComponent.hpp"
#include "components/ChildFileComponent.hpp"
#include "components/PageContent.hpp"
#include "components/ParentSite.hpp"
#include "components/PathComponent.hpp"
#include "components/Site.hpp"
#include "components/HtmlComponent.hpp"

#include "systems/config.hpp"
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
      registry.view<const OriginPathComponent, const DirectoryComponent, ChildFileComponent>();

  // NOTE Feature: Allow multiple sites and get ParentSite from dirEntity
  auto siteEntity = registry.view<const SiteComponent>().front();

  directoriesView.each([&registry, &siteEntity](const auto dirEntity,
                                                const auto &originPath,
                                                auto &children
                                                ) {
    for (auto const &dirEntry :
         std::filesystem::directory_iterator{originPath.path}) {

      if (!std::filesystem::is_regular_file(dirEntry) ||
          dirEntry.path().filename() == "config") {
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
      } else if(pathExtension == ".html") {
        registry.emplace<HTMLComponent>(fileEntity);
      }

      registry.emplace<OriginPathComponent>(fileEntity, dirEntry.path());
      // TODO replace or add relativePathComponent?

      registry.emplace<PageContentComponent>(fileEntity);
    }
  });
}

void generateContent(entt::registry &registry) {
  const auto markdownView =
      registry.view<const OriginPathComponent, PageContentComponent,
                    const MarkdownComponent, const FileComponent>();

  markdownView.each([](const auto &originPath, auto &pageContent) {
    std::ifstream mdFile(originPath.path, std::ios::binary);

    if (mdFile.is_open()) {
      std::stringstream ss;

      ss << mdFile.rdbuf();

      auto mdContent = ss.str();

      pageContent.content = std::string{
          cmark_markdown_to_html(mdContent.c_str(), mdContent.size(), 0)};
    }
  });
}

void clearDirectory(std::filesystem::path directory) {
  for (auto const &dirEntry : std::filesystem::directory_iterator{directory}) {
    if (dirEntry.path().filename() != "assets") {
      std::filesystem::remove_all(dirEntry.path());
    }
  }
}

void outputContent(entt::registry &registry) {

  const auto directoryView =
      registry.view<const OriginPathComponent, const DirectoryComponent>();

  const std::filesystem::path pagesPath{"pages"};

  clearDirectory(std::filesystem::path("public"));

  std::filesystem::create_directory("public");

  directoryView.each([&pagesPath](const auto &originPath) {
    auto destinationPath = std::filesystem::path(
        "public/" +
        std::filesystem::relative(originPath.path, pagesPath).string());

    std::filesystem::create_directory(destinationPath);
  });

  const auto contentView =
      registry.view<const GeneratedContentComponent, const OriginPathComponent,
                    FileComponent>();

  const auto size = contentView.size_hint();

  std::cout << "writing " << size << " files" << std::endl;

  contentView.each(
      [&pagesPath](const auto &generatedContent, const auto &originPath) {
        auto destinationPath = std::filesystem::path(
            "public/" +
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
}

} // namespace cppaper

int main(int argc, char *argv[]) {
  using namespace cppaper;

  entt::registry registry;

  // TODO Refactoring name of system functions to System Acronym and move to
  // properly directories "systems"
  getSite(registry);

  loadSiteDirectories(registry);

  loadSiteFiles(registry);

  configSystem(registry);

  titleSystem(registry);

  // TODO make a site map and add it to template config(only for
  // GeneratedContentComponent)

  generateContent(registry);

  templateSystem(registry);

  outputContent(registry);

  std::cout << "Done!" << std::endl;

  return 0;
}
