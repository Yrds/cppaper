#ifndef CHILDFILE_COMPONENT_INCLUDED
#define CHILDFILE_COMPONENT_INCLUDED

#include "entt/entt.hpp"
#include <vector>

namespace cppaper {
  struct ChildFileComponent {
    std::vector<entt::entity> children;
  };
}

#endif //ChildFile_INCLUDED
