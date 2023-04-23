#include "template/functions/getConfigFrom.hpp"

#include "components/Config.hpp"

namespace cppaper {
void getConfigFrom(entt::registry &registry, inja::Environment &env) {

  env.add_callback("getConfigFrom", 1, [&registry](inja::Arguments args) {
    auto id = args.at(0)->get<entt::entity>();

    auto config = registry.get<ConfigComponent>(id);

    return config.map;
  });
}
} // namespace cppaper
