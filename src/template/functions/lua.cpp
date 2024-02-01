#include "template/functions/lua.hpp"
#include "components/SystemConfigComponent.hpp"
#include "sol/state.hpp"
#include "systems/lua.hpp"

namespace cppaper::templ::scripts {
void luaT(entt::registry &registry, inja::Environment &env) {

  env.add_callback("lua", 1, [&registry](inja::Arguments args) {

    auto systemEntity = registry.view<SystemConfigComponent>().front();

    SystemConfigComponent& systemConfig =
        registry.get<SystemConfigComponent>(systemEntity);

    sol::state lua;
    loadScriptsLibraries(systemConfig, lua);

    auto script = args.at(0)->get<std::string>();

    return static_cast<std::string>(lua.script(script));
  });

}
}
