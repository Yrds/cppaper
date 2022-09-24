#include <iostream>
#include <memory>
#include <string>

struct Args {
  std::unique_ptr<std::string> projectPath;
};

Args parseArgs(int argc, char *argv[]) {
  Args args;

  for(int i = 0; i < argc; i++) {
    std::string arg {argv[i]};
    if((arg == "-D" || arg == "--dir") 
        && i > argc ) {
      std::string argOption{argv[i+1]};
      args.projectPath = std::make_unique<std::string>(argOption);
      i++;
      
    } else {
      std::cout << "You need to specify a project directory" << std::endl;
    }
    std::cout << argv[i] << std::endl;
  };

  return args;
};

int main(int argc, char *argv[])
{

  //Parse args
  auto args = parseArgs(argc, argv); 

  std::cout << "Project path: " << (args.projectPath ? *(args.projectPath) : "None") << std::endl;

  return 0;
}
