#include "template.hpp"

#include <fstream>
#include <functional>
#include <sstream>
#include <utility>

#include "Page.hpp"

namespace cppaper {

std::map<std::string, TemplateFunction> templateFunctionsMap = {
    {"pageContent", [](Site &site, Directory &directory, Page &page,
                    std::string args) { 

      return ""; 
    }}};

std::pair<int, int> findPlaceholders(std::string& templateString, int offset = 0) {
  const auto beginBracket = templateString.find("{{ ", offset);
  const auto endBracket = templateString.find(" }}", beginBracket);

  return {beginBracket, endBracket};
};

std::string execTemplate(std::string templateFilePath,
                         const Page &page) { // TODO add Site& reference
  auto templateFile = std::ifstream(templateFilePath);

  std::stringstream templateStream;

  templateStream << templateFile.rdbuf();

  std::string templateString = templateStream.str();

  templateString.replace(templateString.find("{{ $html }}"), 12, page.html);

  return templateString;
}
} // namespace cppaper
