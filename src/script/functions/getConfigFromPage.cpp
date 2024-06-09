#include "components/Config.hpp"
#include "script/functions/getPagesWithConfig.hpp"

namespace cppaper::script::functions {
void getConfigFromPage(entt::registry& registry, sol::state& lua) {
    lua.set_function("get_config_from_page", [&](unsigned int entity_id) -> sol::nested<std::map<std::string, std::string>> {
        entt::entity entity = static_cast<entt::entity>(entity_id);

        const auto& map = registry.get<ConfigComponent>(entity).map;

        return map;
    });
}
}
