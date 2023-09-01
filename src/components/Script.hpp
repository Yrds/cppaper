#ifndef SCRIPT_COMPONENT_INCLUDED
#define SCRIPT_COMPONENT_INCLUDED

#include "entt/entt.hpp"
#include "sol/sol.hpp"

namespace cppaper {
struct ScriptComponent {
  sol::state lua;
};
} // namespace cppaper

#endif // Script_INCLUDED
