#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "Site.hpp"
#include "cmark-gfm.h"
#include "config.hpp"
#include "template.hpp"

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
      if (pathExtension ==  ".md" || pathExtension == ".html") {
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
    pages.push_back(
        {mdFile.stem(), getHTMLFromMD(mdFile), mdFile, getConfig(mdFile)});
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
        ss << page.html;
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

Site getSite() {
  std::filesystem::path sitePath("");

  return Site{
      getConfig(sitePath),
  };
}

void loadSiteDirectories(Site &site) { site.directory = getPagesDirectory(); }

void loadSitePages(Site &site) {
  site.directory.pages = getPages(site.directory.path);
}

} // namespace cppaper

int main(int argc, char *argv[]) {

  using namespace cppaper;

  auto site = getSite();

  loadSiteDirectories(site);

  loadSitePages(site);

  auto publicDirectory = createPublicDirectory();

  outputSite(site, publicDirectory);

  return 0;
}
