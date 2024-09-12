#ifndef CPPAPER_SCRIPT_FUNCTIONS_GCFE_INCLUDED
#define CPPAPER_SCRIPT_FUNCTIONS_GCFE_INCLUDED

#include "entt/entt.hpp"
#include "sol/sol.hpp"

namespace cppaper::script::functions {
  void getConfigFromEntity(entt::registry& registry, sol::state& lua);
}

#endif //CPPAPER_SCRIPT_FUNCTIONS_GCFE_INCLUDED
