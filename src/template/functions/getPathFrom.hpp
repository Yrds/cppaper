#ifndef CPPAPER_TEMPLATE_FUNCTIONS_GETPATHFROM
#define CPPAPER_TEMPLATE_FUNCTIONS_GETPATHFROM

#include "inja/inja.hpp"
#include "entt/entt.hpp"

namespace cppaper {
  void getPathFromT(entt::registry& registry, inja::Environment& env);
}
#endif
