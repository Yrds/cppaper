#include "script/functions/getPagesWithConfig.hpp"

#include "systems/config.hpp"


namespace cppaper::script::functions {

  void getPagesWithConfig(entt::registry& registry, sol::state& lua) {
    lua.set_function("get_pages_with_config", [](sol::variadic_args va) -> std::vector<unsigned int> {
        if (va.size() > 2) {
          throw std::invalid_argument("This function requires 2 arguments at maximum.");
        }

        std::string config = va[0];


        std::vector<unsigned int> ret{};
        if (va.size() == 1) {
            for (const auto el: cppaper::getPagesWithConfig(config)) {
                ret.push_back(static_cast<unsigned int>(el));
            };
            return ret;
        }

        std::string value = va[1];
        for (const auto el: cppaper::getPagesWithConfig(config, value)) {
            ret.push_back(static_cast<unsigned int>(el));
        };

        return ret;
    });

  }
}
