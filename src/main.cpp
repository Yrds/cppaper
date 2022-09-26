#include <iostream>
#include <memory>
#include <string>
#include <filesystem>
#include "PARSE_ERROR.hpp"



struct Args {
  static std::string DEFAULT_FILEPATH;
  std::unique_ptr<std::string> projectPath;
  std::string outputPath = DEFAULT_FILEPATH;
};

std::string Args::DEFAULT_FILEPATH = "./output";

//Parse all the command line parameters
Args parseArgs(int argc, char *argv[]) {
  Args args;

  for(int i = 0; i < argc; i++) {
    std::string arg {argv[i]};
    if(arg == "-D" || arg == "--dir") {
      if(i < argc - 1) {
        std::string argOption{argv[i+1]};
        args.projectPath = std::make_unique<std::string>(std::string(argOption));
        i++;
      }
      continue;
    }

    if(arg == "-O" || arg == "--output") {
      if(i < argc - 1) {
        std::string argOption{argv[i+1]};
        args.outputPath = std::string(argOption);
        i++;
      }
      continue;
    }
    std::cout << argv[i] << std::endl;
  };

  return args;
};

//Validate and return an error if something went wrong otherwise 0 || PARSE_ERROR
PARSE_ERROR validateArgs(Args& args){ 
  std::cout << !args.projectPath << std::endl;
  if(!args.projectPath) {
    return NO_PROJECT_DIR;
  }

  return NO_ERROR;
}


int main(int argc, char *argv[])
{

  //Parse args
  Args args = parseArgs(argc, argv); 

  auto validation = validateArgs(args);

  if(validation == NO_PROJECT_DIR) {
    std::cout << "No project directory specified" << std::endl; 
    return validation;
  }

  if(args.outputPath == Args::DEFAULT_FILEPATH) {
    std::cout << "No output path specified. Using \"" << Args::DEFAULT_FILEPATH << "\"" << std::endl;
  } else {
    std::cout << "Output path: " << args.outputPath << std::endl;
  }

  auto projectPath = std::filesystem::absolute(std::filesystem::path((*(args.projectPath))));

  if(std::filesystem::is_directory(projectPath)) {
    std::cout << "Project path: " << std::filesystem::absolute(projectPath) << std::endl;

  }

  //TODO read all files and respective paths
  //
  //TODO create page based on .md on outputPath
  //
  //TODO cleanUp possible files

  return 0;
}
