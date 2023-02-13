#include <fstream>
#include "systems/json.hpp"
#include "components/PathComponent.hpp"
#include "components/JSONComponent.hpp"

#include "inja/inja.hpp"

namespace cppaper {
void jsonSystem(entt::registry &registry) {
  auto jsonFileView = registry.view<JSONComponent, OriginPathComponent>();

  jsonFileView.each([](auto& jsonComponent, const auto &pathComponent) {
      std::ifstream file(pathComponent.path);
      jsonComponent.data = nlohmann::json::parse(file);
  });
}
} // namespace cppaper
