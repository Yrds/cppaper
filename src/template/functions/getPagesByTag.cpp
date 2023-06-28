#include "template/functions/getPagesByTag.hpp"

#include "systems/tag.hpp"

namespace cppaper {
void getPagesByTagT(entt::registry& registry, inja::Environment& env) {
  env.add_callback("getPagesByTag", 1, [](inja::Arguments args) {
    std::string tag = args.at(0)->get<std::string>();

    return std::vector<entt::entity>(getPagesByTag(tag));
  });
}
}
