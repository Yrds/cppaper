#ifndef LUA_SYSTEM_INCLUDED
#define LUA_SYSTEM_INCLUDED

#include "components/SystemConfigComponent.hpp"
#include "entt/entt.hpp"
#include "sol/sol.hpp"

namespace cppaper {
// Note: should run before sitemap and output
void scanScriptFiles(entt::registry &registry);
void initScriptSystem(entt::registry &registry);

void loadScriptsLibraries(SystemConfigComponent& systemConfigComponent, sol::state& lua);

void luaBeforeTemplate(entt::registry &registry);
void luaBeforeOutput(entt::registry &registry);
}

#endif
