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

#include "lib/ninja/ninja_syntax.hpp"

#include "entt/entt.hpp"

#include "commands.hpp"

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

        std::cout << dirEntry << '\n';

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

enum class NinjaBuildError : uint8_t {
  kCppaperLuaNotFound = 1,
  kNinjaBuildFileCreationFailed,
  kCppaperIsNotRegularFile,
};

auto create_ninja_build(entt::registry &reg) -> std::expected<void, NinjaBuildError> {
  // TODO (yuri): read cppaper.lua file to generate build.ninja
  sol::state lua;
  lua.open_libraries(sol::lib::base, sol::lib::package);
  std::filesystem::path cppaper_lua { "cppaper.lua" };

  if (!std::filesystem::exists(cppaper_lua)) {
    return std::unexpected(NinjaBuildError::kCppaperLuaNotFound);
  }

  if (!std::filesystem::is_regular_file(cppaper_lua)) {
    return std::unexpected(NinjaBuildError::kCppaperIsNotRegularFile);
  }

  lua.script_file(cppaper_lua);

  // TODO (yuri): regenerate only if something changed
  const auto writer = ninja_lib::Writer::create(
    std::make_unique<std::ofstream>("build.ninja"),
    2);

  if (!writer) {
    // TODO(yuri): use expected instead of throwing
    throw std::invalid_argument("Failed to create ninja build file");
  }

  writer->comment("Ninja build file generated by Cppaper");

  writer->rule("pool", std::string_view{ "console" });
  writer->variable("rule", std::string_view{ "cp" });
  writer->variable("command", std::string_view{ "cp $in $out" });

  writer->new_line(1);

  auto site_copy_outputs = std::vector<std::string_view>{
    "site_copy"
  };
  writer->build(site_copy_outputs, std::string_view{ "rule" });

  std::system("ninja -f build.ninja");

  return {};
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


  std::cout << "writing " << size << " files (excluded files are counting to that too)" << '\n';

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

void cmd_line_parse(std::string arg, std::function<void(std::string)> callback,
                  int argc, char *argv[]) {
  for (auto argi = 0; argi < argc; argi++) {
    if (std::string(argv[argi]) == arg && (argi + 1 <= argc)) {
      callback(std::string(argv[argi + 1]));
    }
  }
}

enum class CommandError: uint8_t {
  kUnknownCommand = 1,
};

auto cmd_line_parse(const std::string& arg, std::function<void()> callback,
                  int argc, char *argv[]) -> std::expected<void, CommandError> {

  for (auto argi = 0; argi < argc; argi++) {
    if (std::string(argv[argi]) == arg && (argi + 1 <= argc)) {
      callback();
      return {};
    }
  }

  return std::unexpected(CommandError::kUnknownCommand);
}

// TODO(yuri): config system?
void setSystem(entt::registry &registry) {
  using namespace cppaper;

  const auto systemEntity = registry.create();
  registry.emplace<SystemConfigComponent>(systemEntity);
}

namespace {

auto process_commands(int argc, char** argv, entt::registry &registry) -> std::expected<void, CommandError> {
  auto command_entity = registry.create();

  if (const auto result = cmd_line_parse(
    "build", [&]() {
      registry.emplace<cppaper::Command>(command_entity);
      // TODO(yuri): now it should read a cppaper.lua file and generate the ninja build file
      // no more automatic reading pages
      // everything should be done via cppaper.lua
      // everything should be explicit on the build file
      // offers easy functions but also don't hide complexity in case user wants to do something custom
      const auto result = cppaper::create_ninja_build(registry);

      if (result) {
        return;
      }

      if (result.error() == cppaper::NinjaBuildError::kCppaperLuaNotFound) {
        std::cerr << "[ERROR] cppaper.lua not found" << '\n';
      } else if (result.error() == cppaper::NinjaBuildError::kCppaperIsNotRegularFile) {
        std::cerr << "[ERROR] cppaper.lua is not a regular file" << '\n';
      }
    },
  argc, argv); result) {
    return result;
  };

  return std::unexpected(CommandError::kUnknownCommand);
} // namespace

}

auto main(int argc, char **argv, char **  /*envp*/) -> int try {
  using namespace cppaper;

  entt::registry registry;

  setSystem(registry);

  cmd_line_parse(
      "-C", [](const std::string& value) { std::filesystem::current_path(value); },
      argc, argv);

  auto result = process_commands(argc, argv, registry);

  if (result) {
    return 0;
  }

  if (result.error() == CommandError::kUnknownCommand) {
    std::cout << "No command specified, running default build process..." << '\n';
  } else {
    std::cerr << "[ERROR] Unknown command error" << '\n';
    return 1;
  }

  return static_cast<int>(result.error());



  cmd_line_parse(
      "-O",
      [&registry](const std::string& value) -> void {
        const auto system_entity =
            registry.view<SystemConfigComponent>().front();

        auto &system_config = registry.get<SystemConfigComponent>(system_entity);

        system_config.publicDirectory = std::filesystem::path{value + "/"};
      },
      argc, argv);

  /*
  std::cout << "Getting site" << '\n';
  getSite(registry);

  std::cout << "Loading directories" << '\n';;
  loadSiteDirectories(registry);

  std::cout << "Reading files" << '\n';
  loadSiteFiles(registry);

  std::cout << "Reading configuration" << '\n';
  configSystem(registry);

  std::cout << "Scanning script files" << '\n';
  scanScriptFiles(registry);

  std::cout << "Initializing script system" << '\n';
  initScriptSystem(registry);

  std::cout << "Processing extensions" << '\n';
  extensionSystem(registry);

  std::cout << "Reading file contents" << '\n';
  readFilesContent(registry);

  std::cout << "Finding 'no_output' files" << '\n';
  noOutputValidation(registry);

  std::cout << "Indexing tags" << '\n';
  createTagIndex(registry);

  std::cout << "Parsing relative path" << '\n';
  relativePathSystem(registry);

  std::cout << "Updating relative Paths Output extensions" << '\n';
  setFilesRelativePathExtension(registry);

  std::cout << "Mounting directories map" << '\n';
  directoriesMapSystem(registry);

  std::cout << "Parsing JSON Files" << '\n';
  jsonSystem(registry);

  std::cout << "Running Indexing System" << '\n';
  indexSystem(registry);

  std::cout << "Title System" << '\n';
  titleSystem(registry);

  std::cout << "[SCRIPT] Before Templating" << '\n';
  luaBeforeTemplate(registry);

  // TODO(yuri): implement shortcodes

  std::cout << "Initialiazing template environment" << '\n';
  initTemplateEnvironment(registry);

  // TODO(yuri): generate TemplateComponent
  std::cout << "Templating content" << '\n';
  templateFileContent(registry);

  std::cout << "Generating Content" << '\n';
  markdownSystem(registry);

  std::cout << "Mounting templates" << '\n';
  templateSystem(registry);

  std::cout << "[SCRIPT] Before Output" << '\n';
  luaBeforeOutput(registry);

  std::cout << "Generating sitemap" << '\n';
  sitemapSystem(registry);
  */

  // TODO(yuri): output ninja file instead of directly outputting content
  //outputContent(registry);


  std::cout << "Done!" << '\n';

  return 0;
} catch (const std::exception &ex) {
  std::cerr << "[ERROR] " << ex.what() << '\n';
  return 1;
} 
