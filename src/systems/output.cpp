#include "components/Config.hpp"
#include "components/FileComponent.hpp"
#include "components/NoOutput.hpp"
#include "systems/output.hpp"

#include <iostream>

namespace cppaper {
void noOutputValidation(entt::registry &registry) {
  auto fileView = registry.view<const ConfigComponent, const FileComponent>().use<FileComponent>();

  fileView.each([&registry](const auto fileEntity, const auto &configComponent) {
      if (auto noOutput = configComponent.map.find("no_output");
          noOutput != configComponent.map.end() &&
          noOutput->second == "true") {
      std::cout << "Adding no_output " << std::endl;
        registry.emplace<NoOutputComponent>(fileEntity);
      }
  });
}
} // namespace cppaper
