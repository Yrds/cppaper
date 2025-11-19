#include "command.hpp"
#include "components/Command.hpp"

namespace cppaper {

auto command_system(entt::registry& registry) -> void {
  auto build_command_view = registry.view<Command, BuildCommand>();

  build_command_view.each([&](auto entity) -> void {
  });
}

}
