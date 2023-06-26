#ifndef TEMPLATE_FUNCTIONS_GETPAGESBYTAG_INCLUDED
#define TEMPLATE_FUNCTIONS_GETPAGESBYTAG_INCLUDED

#include "inja/inja.hpp"
#include "entt/entt.hpp"

namespace cppaper {
  void getPagesByTagT(entt::registry& registry, inja::Environment& env);
}
#endif //TEMPLATE_FUNCTIONS_GETPAGESBYTAG_INCLUDED

