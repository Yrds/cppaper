#include "script/functions/getPagesWithConfig.hpp"

#include "systems/config.hpp"

namespace cppaper::script::functions {
  void getPagesWithConfig(entt::registry& registry, sol::state& lua) {
    lua.set_function("get_pages_with_config", [](sol::variadic_args va) {
        if (va.size() > 2) {
          throw std::invalid_argument("This function requires 2 arguments at maximum.");
        }

        std::string config = va[0];
        if (va.size() == 1) {
          return std::vector<entt::entity>(cppaper::getPagesWithConfig(config));
        }

        std::string value = va[1];
        return std::vector<entt::entity>(cppaper::getPagesWithConfig(config, value));
    });

  }
}
