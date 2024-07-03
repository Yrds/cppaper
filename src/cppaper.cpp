#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <string>
#include <ranges>
#include <set>

#include "Site.hpp"

#include "components/SystemConfigComponent.hpp"
#include "components/NoOutput.hpp"
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
#include "components/SitemapComponent.hpp"
#include "components/RelativePath.hpp"

#include "systems/config.hpp"
#include "systems/directoriesMap.hpp"
#include "systems/fileContent.hpp"
#include "systems/index.hpp"
#include "systems/json.hpp"
#include "systems/lua.hpp"
#include "systems/markdown.hpp"
#include "systems/output.hpp"
#include "systems/relativePath.hpp"
#include "systems/sitemap.hpp"
#include "systems/template.hpp"
#include "systems/title.hpp"
#include "systems/extension.hpp"
#include "systems/tag.hpp"
#include "systems/fileContent.hpp"
#include "systems/sitemap.hpp"

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

    std::set<std::filesystem::path> directories{{"pages"}};

    while (!directories.empty()) {
      const auto directory = *directories.begin();
      for (auto const &dirEntry :
        std::filesystem::directory_iterator{directory}) {

        std::cout << dirEntry << std::endl;

        if (std::filesystem::is_directory(dirEntry)) {
          const auto directoryEntity = registry.create();

          registry.emplace<DirectoryComponent>(directoryEntity);
          registry.emplace<ChildFileComponent>(directoryEntity);
          registry.emplace<ParentSite>(directoryEntity, siteEntity);

          registry.emplace<OriginPathComponent>(directoryEntity,
                                                dirEntry.path());

          directories.insert(dirEntry.path());
        }
      }

      directories.erase(directory);
    }
  });
}

void loadSiteFiles(entt::registry &registry) {
  const auto directoriesView =
    registry.view<const OriginPathComponent, const DirectoryComponent,
    ChildFileComponent>();

  const auto siteEntity = registry.view<const SiteComponent>().front();

  directoriesView.each(
    [&registry, &siteEntity](
      const auto dirEntity,
      const auto &originPath,
      auto &children
    ) {
      std::cout << "Reading directory: " << originPath.path << std::endl;

      //TODO use std::ranges::to<std::set> in the future
      const auto sortedDirectories = [=]() -> std::set<std::filesystem::path> {
        std::set<std::filesystem::path> sorted;

        for (auto const &dirEntry : std::filesystem::directory_iterator{originPath.path}) {
          sorted.insert(dirEntry.path());
        }

        return sorted;
      }();

      for (const auto& path: sortedDirectories) {
        if (!std::filesystem::is_regular_file(path) ||
          path.filename() == "config" ||
          path.filename().string().ends_with(".config")) {
          continue;
        }

        std::cout << path << std::endl;

        const auto fileEntity = registry.create();

        registry.emplace<FileComponent>(fileEntity);
        registry.emplace<ParentSite>(fileEntity, siteEntity);
        registry.emplace<ParentDirectoryComponent>(fileEntity, dirEntity);
        children.children.push_back(fileEntity);
        registry.emplace<OriginPathComponent>(fileEntity, path);
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
                    FileComponent>(entt::exclude<NoOutputComponent>);

  const auto rawFileView =
      registry.view<const OriginPathComponent, const RawFileComponent>(entt::exclude<NoOutputComponent>);

  const auto indexFileView =
      registry.view<const ParentDirectoryComponent, const IndexFileComponent,
                    const GeneratedContentComponent>(entt::exclude<NoOutputComponent>);

  const auto sitemapFileView =
      registry.view<const GeneratedContentComponent, const RelativePathComponent, const SitemapComponent>();

  sitemapFileView.each([&publicDirectory](const auto &generatedContentComponent, const auto &relativePathComponent){
    auto destinationPath = publicDirectory;
    destinationPath += relativePathComponent.path;

    std::ofstream outputSitemapFile(destinationPath);

    if(outputSitemapFile.is_open()) {
      std::stringstream ss;

      ss << generatedContentComponent.content;

      outputSitemapFile << ss.rdbuf();
    } else {
      // throw "Failed to write file " + destinationPath.string();
      throw std::invalid_argument("Failed to write file: " +
                                  destinationPath.string());
    }
  });

  const auto size = contentView.size_hint() + rawFileView.size_hint() +
                    indexFileView.size_hint() + sitemapFileView.size_hint();


  std::cout << "writing " << size << " files (excluded files are counting to that too)" << std::endl;

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
  // IDEA 1: Don't remove rawFiles(see clearDirectory function)
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

  std::cout << "Getting site" << std::endl;

  getSite(registry);

  std::cout << "Loading directories" << std::endl;;
  loadSiteDirectories(registry);

  std::cout << "Reading files" << std::endl;
  loadSiteFiles(registry);

  std::cout << "Reading configuration" << std::endl;
  configSystem(registry);

  std::cout << "Scanning script files" << std::endl;
  scanScriptFiles(registry);

  std::cout << "Initializing script system" << std::endl;
  initScriptSystem(registry);

  std::cout << "Processing extensions" << std::endl;
  extensionSystem(registry);

  std::cout << "Reading file contents" << std::endl;
  readFilesContent(registry);

  std::cout << "Finding 'no_output' files" << std::endl;
  noOutputValidation(registry);

  std::cout << "Indexing tags" << std::endl;
  createTagIndex(registry);

  std::cout << "Parsing relative path" << std::endl;
  relativePathSystem(registry);

  std::cout << "Updating relative Paths Output extensions" << std::endl;
  setFilesRelativePathExtension(registry);

  std::cout << "Mounting directories map" << std::endl;
  directoriesMapSystem(registry);

  std::cout << "Parsing JSON Files" << std::endl;
  jsonSystem(registry);

  std::cout << "Running Indexing System" << std::endl;
  indexSystem(registry);

  std::cout << "Title System" << std::endl;
  titleSystem(registry);

  std::cout << "[SCRIPT] Before Templating" << std::endl;
  luaBeforeTemplate(registry);

  //TODO implement shortcodes

  std::cout << "Initialiazing template environment" << std::endl;
  initTemplateEnvironment(registry);

  //TODO generate TemplateComponent
  std::cout << "Templating content" << std::endl;
  templateFileContent(registry);

  std::cout << "Generating Content" << std::endl;
  markdownSystem(registry);

  std::cout << "Mounting templates" << std::endl;
  templateSystem(registry);

  std::cout << "[SCRIPT] Before Output" << std::endl;
  luaBeforeOutput(registry);

  std::cout << "Generating sitemap" << std::endl;
  sitemapSystem(registry);

  // TODO markdown output html on output content when there is no template
  std::cout << "Writing content" << std::endl;
  outputContent(registry);

  std::cout << "Done!" << std::endl;

  return 0;
} catch (const std::exception &ex) {
  std::cerr << "[ERROR] " << ex.what() << std::endl;
  return 1;
} 
