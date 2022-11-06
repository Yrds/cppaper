#include "cmark-gfm.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

std::filesystem::path getPagesDirectory(std::string pagesDirectoryPath = "pages") {
  //TODO exception if pages is not a directory
  return std::filesystem::path(pagesDirectoryPath);
}

std::vector<std::filesystem::path> getMdFiles(std::filesystem::path dirPath) {
  //TODO dirPath must be a dir, otherwise exception
  std::vector<std::filesystem::path> mdFiles;

  for(auto const& dir_entry : std::filesystem::directory_iterator{dirPath}) {
    if(dir_entry.path().extension().string() == ".md") {
      mdFiles.push_back(dir_entry.path());
    }
  }

  return mdFiles;
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

bool generateMdToHTML(std::filesystem::path& mdPath, std::filesystem::path& htmlPath) {

  std::ifstream mdFile(mdPath, std::ios::binary);

  //TODO Find a better way to write this code(It's a mess!)
  if(mdFile.is_open()) {
    char charName;

    std::string mdContent;

    while(mdFile.get(charName)){
      mdContent = mdContent + charName;
    }

    auto htmlContent = std::string{cmark_markdown_to_html(mdContent.c_str(), mdContent.size(), 0)};

    std::cout << mdPath << std::endl;

    std::cout << htmlContent << std::endl;
  } else {
    return false;
  }


  //TODO output content to
  //
  return true;
}

int main(int argc, char *argv[])
{

  auto pagesDirectory = getPagesDirectory();

  auto mdFiles = getMdFiles(pagesDirectory);

  auto publicDirectory = createPublicDirectory();

  for(auto& mdFile: mdFiles) {
    generateMdToHTML(mdFile, publicDirectory);
  }



  //TODO convert all .md files to html
  //TODO output html to 'public' directory

  return 0;
}
