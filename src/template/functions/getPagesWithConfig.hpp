#ifndef CPPAPER_TEMPLATE_FUNCTIONS_GETPAGESWITHCONFIG
#define CPPAPER_TEMPLATE_FUNCTIONS_GETPAGESWITHCONFIG

#include "inja/inja.hpp"
#include "entt/entt.hpp"

namespace cppaper {
  void getPagesWithConfig(entt::registry& registry, inja::Environment& env);
}
#endif
