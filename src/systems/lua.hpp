#ifndef LUA_SYSTEM_INCLUDED
#define LUA_SYSTEM_INCLUDED

#include "entt/entt.hpp"

namespace cppaper {
// Note: should run before sitemap and output
void scanScriptFiles(entt::registry &registry);
void initScriptSystem(entt::registry &registry);
void luaBeforeOutput(entt::registry &registry);
}

#endif
