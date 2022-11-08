#include "template.hpp"
#include "Directory.hpp"
#include "inja/inja.hpp"

#include <fstream>
#include <functional>
#include <sstream>
#include <utility>

#include "Page.hpp"

namespace cppaper {

std::string execTemplate(std::string templateFilePath, const Page &page,
                         const Directory &directory,
                         const Site &site) { // TODO add Site& reference

  inja::Environment env; // TODO maybe make env as a reference, so doesn't need
                         // to be included in every iteration with a page

  inja::Template templ = env.parse_template(templateFilePath);

  inja::json data;
  // TODO include partials layouts to be included

  data["page"]["html"] = page.html;

  return env.render(templ, data);
}
} // namespace cppaper
