#include "template/functions/getPagesWithConfig.hpp"

#include "systems/config.hpp"

namespace cppaper {
  void getPagesWithConfig(entt::registry& registry, inja::Environment& env) {

    env.add_callback("getPagesWithConfig", 1, [](inja::Arguments args) {
      auto config = args.at(0)->get<std::string>();

      return std::vector<entt::entity>(getPagesWithConfig(config));
    });

    env.add_callback("getPagesWithConfig", 2, [](inja::Arguments args) {
      auto config = args.at(0)->get<std::string>();
      auto value = args.at(1)->get<std::string>();

      return std::vector<entt::entity>(getPagesWithConfig(config, value));
    });

  }
}
