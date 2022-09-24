#include <iostream>
#include <memory>
#include <string>
#include "PARSE_ERROR.hpp"

struct Args {
  std::unique_ptr<std::string> projectPath;
};

//Parse all the command line parameters
Args parseArgs(int argc, char *argv[]) {
  Args args;

  for(int i = 0; i < argc; i++) {
    std::string arg {argv[i]};
    if((arg == "-D" || arg == "--dir") 
        && i > argc ) {
      std::string argOption{argv[i+1]};
      args.projectPath = std::make_unique<std::string>(argOption);
      i++;
    }
    //std::cout << argv[i] << std::endl;
  };

  return args;
};

//Validate and return an error if something went wrong otherwise 0 || PARSE_ERROR
PARSE_ERROR validateArgs(Args& args){ 
  if(!args.projectPath) {
    return NO_PROJECT_DIR;
  }

  return NO_ERROR;
}

int main(int argc, char *argv[])
{

  //Parse args
  Args args = parseArgs(argc, argv); 

  auto validation = validateArgs(args) == NO_ERROR;
  if(validation == NO_PROJECT_DIR) {
    std::cout << "No project dir specified" << std::endl; 
    return validation;
  } else {
    std::cout << "Project path: " << (args.projectPath ? *(args.projectPath) : "None") << std::endl;
  }


  return 0;
}
