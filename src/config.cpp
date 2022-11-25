#include <fstream>

#include "config.hpp"

namespace cppaper {
  ConfigMap getConfig(const std::filesystem::path &directory) {
    ConfigMap directoryConfig;

    std::ifstream configFile;

    if (std::filesystem::is_directory(directory)) {
      configFile.open(directory.string() + "/config");
    } else {
      configFile.open(directory.string() + ".config");
    }

    if (configFile.is_open()) {
      std::string line;
      while (std::getline(configFile, line)) {
        auto delimiterPosition = line.find("=");
        if (delimiterPosition != std::string::npos) {
          directoryConfig[line.substr(0, delimiterPosition)] =
            line.substr(delimiterPosition + 1);
        }
      }
    }

    return directoryConfig;
  }

  std::filesystem::path getTemplateDirectory() {
    return std::filesystem::path("templates/");
  }
}
