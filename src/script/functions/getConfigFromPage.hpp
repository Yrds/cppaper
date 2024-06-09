#ifndef CPPAPER_SCRIPT_FUNCTIONS_GET_CONFIG_FROM_PAGE_HPP_INCLUDED
#define CPPAPER_SCRIPT_FUNCTIONS_GET_CONFIG_FROM_PAGE_HPP_INCLUDED

#include "entt/entt.hpp"
#include "sol/sol.hpp"

namespace cppaper::script::functions {
  void getConfigFromPage(entt::registry& registry, sol::state& lua);
}

#endif //CPPAPER_SCRIPT_FUNCTIONS_GET_CONFIG_FROM_PAGE_HPP_INCLUDED
