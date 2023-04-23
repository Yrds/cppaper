#ifndef CPPAPER_TEMPLATE_FUNCTIONS_GETCONFIGFROM
#define CPPAPER_TEMPLATE_FUNCTIONS_GETCONFIGFROM

#include "inja/inja.hpp"
#include "entt/entt.hpp"

namespace cppaper {
  void getConfigFrom(entt::registry& registry, inja::Environment& env);
}
#endif
