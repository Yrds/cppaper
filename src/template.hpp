#ifndef TEMPLATE_HPP_INCLUDED
#define TEMPLATE_HPP_INCLUDED

#include <string>
#include <functional>

#include "Site.hpp"

namespace cppaper {
  std::string execTemplate(std::string templateFilePath,
      const Page &page, const Directory& directory, const Site& site);
}

#endif //TEMPLATE_INCLUDED
