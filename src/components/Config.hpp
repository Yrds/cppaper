#ifndef COMPONENTS_CONFIG_HPP_INCLUDED
#define COMPONENTS_CONFIG_HPP_INCLUDED

#include <map>
#include <string>

namespace cppaper {
  struct ConfigComponent {
    std::map<std::string, std::string> map;
  };
};

#endif //COMPONENTS_CONFIG_HPP_INCLUDED
