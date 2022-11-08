#include "template.hpp"
#include "inja/inja.hpp"

#include <fstream>
#include <functional>
#include <sstream>
#include <utility>

#include "Page.hpp"

namespace cppaper {

std::string execTemplate(std::string templateFilePath,
                         const Page &page) { // TODO add Site& reference
  inja::Environment env;

  inja::Template templ = env.parse_template(templateFilePath);

  inja::json data;
  data["html"] = page.html;

  return env.render(templ, data);
}
} // namespace cppaper
