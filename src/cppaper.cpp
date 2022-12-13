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
#include "config.hpp"
#include "template.hpp"

#include "components/Config.hpp"
#include "components/DirectoryComponent.hpp"
#include "components/FileComponent.hpp"
#include "components/GeneratedContentComponent.hpp"
#include "components/MarkdownComponent.hpp"
#include "components/PageContent.hpp"
#include "components/ParentSite.hpp"
#include "components/PathComponent.hpp"
#include "components/Site.hpp"

#include "systems/template.hpp"

#include "entt/entt.hpp"

// NOTE Always before a release:
// const correctness
// format code

namespace cppaper {

Directory getPagesDirectory(std::string pagesDirectoryPath = "pages") {

  std::filesystem::path directoryPath{pagesDirectoryPath};

  if (std::filesystem::is_directory(directoryPath)) {
    return {std::filesystem::path(pagesDirectoryPath),
            getConfig(directoryPath)};
  } else {
    throw "Path is not a directory: " + pagesDirectoryPath;
  }
}

std::vector<std::filesystem::path> getMdFiles(std::filesystem::path dirPath) {
  if (std::filesystem::is_directory(dirPath)) {
    std::vector<std::filesystem::path> mdFiles;

    for (auto const &dirEntry : std::filesystem::directory_iterator{dirPath}) {
      auto const pathExtension = dirEntry.path().extension().string();
      if (pathExtension == ".md" || pathExtension == ".html") {
        mdFiles.push_back(dirEntry.path());
      }
    }

    return mdFiles;

  } else {
    throw "This directory it's not a path: " + dirPath.string();
  }
}

std::vector<std::filesystem::path>
getDirectories(std::filesystem::path dirPath) {
  std::vector<std::filesystem::path> directories;

  directories.push_back(dirPath);

  for (auto const &dirEntry : std::filesystem::directory_iterator{dirPath}) {
    if (std::filesystem::is_directory(dirEntry)) {
      directories.push_back(dirEntry.path());
    }
  }

  return directories;
}

std::filesystem::path createPublicDirectory(bool overrideDirectory = true) {
  const std::string publicDirectoryPath = "public";

  std::filesystem::path publicDirectory(publicDirectoryPath);

  if (std::filesystem::exists(publicDirectory) && overrideDirectory) {
    std::filesystem::remove_all(publicDirectory);
  }

  std::filesystem::create_directory(publicDirectoryPath);

  return publicDirectory;
}

std::string getHTMLFromMD(std::filesystem::path &mdFilePath) {

  std::ifstream mdFile(mdFilePath, std::ios::binary);

  // TODO Find a better way to write this code(It's a mess!)
  if (mdFile.is_open()) {
    std::stringstream ss;

    ss << mdFile.rdbuf();

    auto mdContent = ss.str();

    auto htmlContent = std::string{
        cmark_markdown_to_html(mdContent.c_str(), mdContent.size(), 0)};

    return htmlContent;
  }

  throw("Was not possible to open mdFile");
}

std::vector<Page> getPages(std::filesystem::path &directory) {
  std::vector<Page> pages;

  for (auto &mdFile : getMdFiles(directory)) {
    pages.push_back({mdFile.stem(), mdFile, getConfig(mdFile)});
    //{mdFile.stem(), getHTMLFromMD(mdFile), mdFile, getConfig(mdFile)});
  }

  return pages;
}

void outputPages(Directory &pagesDirectory,
                 std::filesystem::path &publicDirectory, Site &site) {
  for (const auto &page : pagesDirectory.pages) {
    auto pagePath = page.path;

    auto fileName = pagePath.replace_extension(".html").string().replace(
        page.path.string().find(pagesDirectory.path.string()),
        pagesDirectory.path.string().size(), publicDirectory.string());

    std::cout << "Writing: " + fileName << std::endl;

    std::ofstream outputPageFile(fileName);

    if (outputPageFile.is_open()) {
      std::stringstream ss;

      if (pagesDirectory.config.contains("template")) {
        ss << execTemplate(getTemplateDirectory().string() +
                               pagesDirectory.config.at("template"),
                           page, pagesDirectory, site);
      } else {
        // TODO ss << page.html;
      }

      outputPageFile << ss.rdbuf();
    } else {
      throw("Can't open file");
    }
  }
}

void outputSite(Site &site, std::filesystem::path &publicDirectory) {
  // TODO Recursive paths to remove first parameter
  outputPages(site.directory, publicDirectory, site);
}

Site getSite(entt::registry &registry) {
  const std::filesystem::path sitePath("");

  const auto site = registry.create();

  registry.emplace<ConfigComponent>(site, getConfig(sitePath));
  registry.emplace<SiteComponent>(site);
  registry.emplace<OriginPathComponent>(site, sitePath);

  return Site{
      getConfig(sitePath),
  };
}

// TODO change this function to scanSiteDirectories
// TODO separate directories scan from file scan
//

void loadSiteDirectories(entt::registry &registry) {
  const auto view = registry.view<OriginPathComponent, SiteComponent>();

  view.each([&registry](const auto siteEntity, const auto &path) {
    std::filesystem::path directoryPath{"pages"};

    // TODO optmize logic of this code

    {
      const auto directoryEntity = registry.create();

      registry.emplace<DirectoryComponent>(directoryEntity);

      registry.emplace<ParentSite>(directoryEntity, siteEntity);

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
      registry.view<const OriginPathComponent, const DirectoryComponent>();

  // NOTE Feature: Allow multiple sites and get ParentSite from dirEntity
  auto siteEntity = registry.view<const SiteComponent>().front();

  directoriesView.each([&registry, &siteEntity](const auto dirEntity,
                                                const auto &originPath) {
    for (auto const &dirEntry :
         std::filesystem::directory_iterator{originPath.path}) {

      if (!std::filesystem::is_regular_file(dirEntry) ||
          dirEntry.path().filename() == "config") {
        continue;
      }

      const auto directoryEntity = registry.create();

      registry.emplace<FileComponent>(directoryEntity);
      registry.emplace<ParentSite>(directoryEntity, siteEntity);
      registry.emplace<ParentDirectoryComponent>(directoryEntity, dirEntity);

      auto const pathExtension = dirEntry.path().extension().string();

      // TODO move this to other system or function inside the same system
      if (pathExtension == ".md") {
        registry.emplace<MarkdownComponent>(directoryEntity);
      }

      registry.emplace<OriginPathComponent>(directoryEntity, dirEntry.path());
      // TODO replace by relativePathComponent?

      registry.emplace<PageContentComponent>(directoryEntity);

      // registry.emplace<ConfigComponent>(directoryEntity,
      //                                   getConfig(dirEntry.path()));
    }
  });
}

void loadSitePages(Site &site) {
  site.directory.pages = getPages(site.directory.path);
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

void loadConfig(entt::registry &registry) {
  auto directoryView =
      registry.view<const ParentSite, const OriginPathComponent,
                    const DirectoryComponent>();

  directoryView.each([&registry](const auto dirEntity, const auto &parentSite,
                                 auto &originPath) {
    auto directoryConfig = getConfig(originPath.path);

    registry.emplace<ConfigComponent>(
        dirEntity, registry.get<ConfigComponent>(parentSite.entity));
    auto dirConfig = registry.get<ConfigComponent>(dirEntity);

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

    auto fileConfig = registry.get<ConfigComponent>(fileEntity);

    for (const auto &[key, value] : tempConfig) {
      fileConfig.map[key] = value;
    }
  });
}

void outputContent(entt::registry &registry) {

  const auto directoryView =
      registry.view<const OriginPathComponent, const DirectoryComponent>();

  const std::filesystem::path pagesPath{"pages"};

  std::filesystem::remove_all("public");
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

          // TODO exec template here
          ss << generatedContent.content;

          outputPageFile << ss.rdbuf();
        } else {
          // throw "Failed to write file " + destinationPath.string();
          throw std::invalid_argument("Failed to write file: " +
                                      destinationPath.string());
        }
      });
  // TODO output content from PageComponent and PageContentComponent and
  // OriginPath
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

  // TODO loadConfig ordering config by this priority File, Directory and then
  // Site(Overriding)

  generateContent(registry);

  // TODO Generate GeneratedContentComponent in this system
  templateSystem(registry);

  outputContent(registry);

  return 0;
}
