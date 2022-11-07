#ifndef TEMPLATE_HPP_INCLUDED
#define TEMPLATE_HPP_INCLUDED

#include <string>
#include <functional>

#include "Site.hpp"

namespace cppaper {
  using TemplateFunction = std::function<std::string(Site&, Directory&, Page&, std::string args)>;

  std::string execTemplate(std::string templateFilePath,
      const Page &page);
}

#endif //TEMPLATE_INCLUDED
