#ifndef TEMPLATE_COMPONENT_INCLUDED
#define TEMPLATE_COMPONENT_INCLUDED

#include "entt/entt.hpp"
#include "inja/inja.hpp"

namespace {
struct TemplateComponent {
  inja::Template template;
  inja::json templateData;
};
}

#endif //Template_INCLUDED
