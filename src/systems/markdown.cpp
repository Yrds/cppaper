#include "systems/markdown.hpp"

#include <string>
#include <sstream>
#include <fstream>

#include <iostream>

#include "cmark-gfm.h"
#include "cmark-gfm-core-extensions.h"
#include "components/FileContentComponent.hpp"
#include "components/PathComponent.hpp"
#include "components/MarkdownComponent.hpp"
#include "components/FileComponent.hpp"
#include "components/Config.hpp"
#include "components/PageContent.hpp"

namespace cppaper {

void attachExtensions(cmark_parser *parser) {

  const char *extension_names[] = {
      "autolink", "strikethrough", "table", "tagfilter", NULL,
  };

  for (const char **it = extension_names; *it; ++it) {
    const char *extension_name = *it;
    cmark_syntax_extension *syntax_extension =
        cmark_find_syntax_extension(extension_name);
    if (!syntax_extension) {
      fprintf(stderr, "%s is not a valid syntax extension\n", extension_name);
      abort();
    }
    cmark_parser_attach_syntax_extension(parser, syntax_extension);
  }
}

void markdownSystem(entt::registry &registry) {

  cmark_gfm_core_extensions_ensure_registered();

  const auto markdownView =
      registry.view<const OriginPathComponent, const MarkdownComponent,
                    const FileComponent, const ConfigComponent,
                    const FileContentComponent>();

  markdownView.each([&registry](const auto entity, const auto &originPath,
                                auto &config, auto &fileContentComponent) {
    std::ifstream mdFile(originPath.path, std::ios::binary);

    if (mdFile.is_open()) {
      int cmark_options = CMARK_OPT_DEFAULT;

      if (auto markdownUnsafe = config.map.find("markdown_unsafe");
          markdownUnsafe != config.map.end() &&
          markdownUnsafe->second == "false") {
        cmark_options ^= CMARK_OPT_UNSAFE;
      }

      cmark_parser *parser = cmark_parser_new(cmark_options);

      attachExtensions(parser);

      cmark_parser_feed(parser, fileContentComponent.content.c_str(),
                        fileContentComponent.content.size());

      cmark_node *doc = cmark_parser_finish(parser);

      registry.emplace<PageContentComponent>(
          entity, std::string{cmark_render_html(doc, cmark_options, NULL)});

      cmark_node_free(doc);
      cmark_parser_free(parser);
    }
  });
}
} // namespace cppaper
