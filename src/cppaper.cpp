#include "cmark-gfm.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <map>


using configMap = std::map<std::string, std::string>;


struct Page {
  std::string title;
  std::string html;
  std::filesystem::path path;
  configMap config;
};

struct Directory {
  std::filesystem::path path;
  configMap config;
  std::vector<Page> pages;
};

struct Site {
  configMap config;
  //std::vector<Directory> directories;
  Directory directory;
};

configMap getConfig(std::filesystem::path& directory) {
  configMap directoryConfig;

  std::ifstream configFile;

  if(std::filesystem::is_directory(directory)) {
    configFile.open(directory.string() + "/config");
  } else {
    configFile.open(directory.string() + ".config");
  }

  if(configFile.is_open()) {
    std::string line;
    while(std::getline(configFile, line)) {
      auto delimiterPosition = line.find("=");
      if(delimiterPosition != std::string::npos) {
        directoryConfig[line.substr(0, delimiterPosition)] = line.substr(delimiterPosition + 1);
      }
    }
  }


  for(const auto& [config, value]: directoryConfig) {
    std::cout << config << ":" << value << std::endl;

  }
  return directoryConfig;
}

Directory getPagesDirectory(std::string pagesDirectoryPath = "pages") {
  
  //TODO exception if pages is not a directory
  std::filesystem::path directoryPath{pagesDirectoryPath};

  return {
    std::filesystem::path(pagesDirectoryPath),
    getConfig(directoryPath)
  };
}

std::vector<std::filesystem::path> getMdFiles(std::filesystem::path dirPath) {
  //TODO dirPath must be a dir, otherwise exception
  std::vector<std::filesystem::path> mdFiles;

  for(auto const& dirEntry : std::filesystem::directory_iterator{dirPath}) {
    if(dirEntry.path().extension().string() == ".md") {
      mdFiles.push_back(dirEntry.path());
    }
  }

  return mdFiles;
}

std::vector<std::filesystem::path> getDirectories(std::filesystem::path dirPath) {

  std::vector<std::filesystem::path> directories;

  directories.push_back(dirPath);

  for(auto const& dirEntry : std::filesystem::directory_iterator{dirPath}) {
    if(std::filesystem::is_directory(dirEntry)) {
      directories.push_back(dirEntry.path());
    }
  }

  return directories;
}

std::filesystem::path createPublicDirectory(bool overrideDirectory = true) {
  const std::string publicDirectoryPath = "public";

  std::filesystem::path publicDirectory(publicDirectoryPath);

  if(std::filesystem::exists(publicDirectory) && overrideDirectory) {
    std::filesystem::remove_all(publicDirectory);
  }

  //TODO exception if directory couldn't be created
  std::filesystem::create_directory(publicDirectoryPath);

  return publicDirectory; 
}

std::string getHTMLFromMD(std::filesystem::path& mdFilePath) {

  std::ifstream mdFile(mdFilePath, std::ios::binary);

  //TODO Find a better way to write this code(It's a mess!)
  if(mdFile.is_open()) {
    std::stringstream ss;

    ss << mdFile.rdbuf();

    auto mdContent = ss.str();

    auto htmlContent = std::string{cmark_markdown_to_html(mdContent.c_str(), mdContent.size(), 0)};

    return htmlContent;
  } 

  throw("Was not possible to open mdFile");
}


std::vector<Page> getPages(std::filesystem::path& directory) {
  std::vector<Page> pages;

  for(auto& mdFile: getMdFiles(directory)) {
    pages.push_back({
        mdFile.stem(),
        getHTMLFromMD(mdFile),
        mdFile,
        getConfig(mdFile)
        });
  }

  return pages;
}

std::string execTemplate(std::string templateFilePath, const Page& page) { //TODO add Site& reference
  auto templateFile = std::ifstream(templateFilePath);

  std::stringstream templateStream;

  templateStream << templateFile.rdbuf();

  std::string templateString = templateStream.str();

  templateString.replace(templateString.find("{{ $html }}"), 12, page.html);

  return templateString;
}

void outputPages(Directory& pagesDirectory, std::filesystem::path& publicDirectory) {
  for(const auto& page: pagesDirectory.pages) {
    auto pagePath = page.path;

    auto fileName = pagePath.replace_extension(".html").string().replace(page.path.string().find(pagesDirectory.path.string()), pagesDirectory.path.string().size(), publicDirectory.string());

    std::cout << "Writing: " + fileName << std::endl;

    std::ofstream outputPageFile(fileName);

    if(outputPageFile.is_open()) {
      std::stringstream ss;

      if(pagesDirectory.config.contains("template")) {

        ss << execTemplate("templates/" + pagesDirectory.config.at("template"), page);

      } else {
        ss << page.html;
      }

      outputPageFile << ss.rdbuf();
    } else {
      throw("Can't open file");
    }

  }
}


//Load configuration and output directory files to public directory
void outputSite(Site& site, std::filesystem::path& publicDirectory) {
  outputPages(site.directory, publicDirectory);
}

Site getSite() {
  std::filesystem::path sitePath("");

  auto pagesDirectory = getPagesDirectory(); 

  return Site {
    getConfig(sitePath),
  };
}

void loadSiteDirectories(Site& site) {
    site.directory = getPagesDirectory();
}

void loadSitePages(Site& site) {
  site.directory.pages = getPages(site.directory.path);
}

int main(int argc, char *argv[])
{

  auto site = getSite();

  loadSiteDirectories(site);

  loadSitePages(site);

  auto publicDirectory = createPublicDirectory();

  outputSite(site, publicDirectory);

  return 0;
}
