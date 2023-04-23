#ifndef CPPAPER_TEMPLATE_FUNCTIONS_GETPAGESFROM
#define CPPAPER_TEMPLATE_FUNCTIONS_GETPAGESFROM

#include "inja/inja.hpp"
#include "entt/entt.hpp"

namespace cppaper {
  void getPagesFrom(entt::registry& registry, inja::Environment& env);
}

#endif
