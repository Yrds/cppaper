#ifndef TEMPLATE_FUNCTIONS_GETJSONFROM_INCLUDED
#define TEMPLATE_FUNCTIONS_GETJSONFROM_INCLUDED

#include "inja/inja.hpp"
#include "entt/entt.hpp"

namespace cppaper {
  void getJsonFrom(entt::registry& registry, inja::Environment& env);

}
#endif //TEMPLATE_FUNCTIONS_GETJSONFROM_INCLUDED
