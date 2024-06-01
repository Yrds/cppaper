#ifndef CPPAPER_SCRIPT_FUNCTIONS_HPP_INCLUDED
#define CPPAPER_SCRIPT_FUNCTIONS_HPP_INCLUDED

#include "entt/entt.hpp"
#include "sol/sol.hpp"

namespace cppaper::script::functions {
  void getPagesWithConfig(entt::registry& registry, sol::state& lua);
}

#endif //CPPAPER_SCRIPT_FUNCTIONS_HPP_INCLUDED
