#include "getJsonFrom.hpp"

#include "components/Config.hpp"
#include "components/JSONComponent.hpp"


namespace cppaper {

void getJsonFrom(entt::registry &registry, inja::Environment &env) {

  env.add_callback("getJsonFrom", 1, [&registry](inja::Arguments args) {
    auto id = args.at(0)->get<entt::entity>();

    auto json = registry.get<JSONComponent>(id);

    return json.data;
  });
}

} // namespace cppaper
