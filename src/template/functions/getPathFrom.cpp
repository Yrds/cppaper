#include "template/functions/getPathFrom.hpp"

#include "components/RelativePath.hpp"


namespace cppaper {
  void getPathFromT(entt::registry& registry, inja::Environment& env) {
    env.add_callback("getPathFrom", 1, [&registry](inja::Arguments args) {
      auto id = args.at(0)->get<entt::entity>();

      auto relativePath = registry.get<RelativePathComponent>(id);

      return relativePath.path;
    });
  }
}
