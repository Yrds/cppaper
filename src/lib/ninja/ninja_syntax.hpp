#ifndef LIB_NINJA_SYNTAX_INCLUDED
#define LIB_NINJA_SYNTAX_INCLUDED

/*
* This is a minimal implementation of ninja build writer system,
* this is entirely based on ninja_syntax.py from ninja build system,
* so if you are familiar with that, you will find this easy to use.
*
* you can check the python version here: https://github.com/ninja-build/ninja/blob/master/misc/ninja_syntax.py
*/

#include <cstdint>
#include <filesystem>
#include <ostream>
#include <memory>
#include <expected>
#include <vector>

namespace ninja_lib {

enum WriterError: uint8_t {
  kUnknownError
};

struct Writer {
  std::unique_ptr<std::ostream> stream;
  uint8_t indent_size = 0;

  static auto create(
    std::unique_ptr<std::ostream> stream,
    uint8_t indent_size
  ) -> std::expected<Writer, WriterError>;

  auto new_line(int count) const -> void;

  auto comment(std::string_view comment_text) const -> void;

  auto variable(std::string_view key) const -> void;
  auto variable(std::string_view key, std::string_view value, int indent = 0) const -> void;
  auto variable(std::string_view key, bool value, int indent = 0) const -> void;
  auto variable(std::string_view key, int value, int indent = 0) const -> void;
  auto variable(std::string_view key, float value, int indent = 0) const -> void;
  auto variable(std::string_view key, double value, int indent = 0) const -> void;
  auto variable(std::string_view key, std::vector<std::string_view>& values, int indent = 0) const -> void;

  void pool(
    std::string_view name,
    int depth,
    int indent = 0
  );

  auto rule(
    std::string_view name,
    std::string_view command,
    std::string_view description = "",
    std::string_view depfile = "",
    bool generator = false,
    std::string_view pool = "",
    bool restat = false,
    std::string_view rspfile = "",
    std::string_view rspfile_content = "",
    std::vector<std::string_view> deps = {}
  ) const -> void;

  auto build(
    const std::vector<std::string_view>& outputs,
    std::string_view rule,
    const std::vector<std::string_view>& inputs = {},
    const std::vector<std::pair<std::string_view, std::string_view>>& variables = {},
    const std::vector<std::string_view>& implicit = {},
    std::string_view pool = {},
    const std::vector<std::string_view>& implicit_outputs = {},
    std::string_view dyndep = ""
  ) const -> void;

  auto include(std::filesystem::path path) -> void;

  auto line(std::string_view line_text, int indent = 0) -> void;

  private:
    Writer() = default;
    auto count_dollars_before_index(std::string_view str, size_t index) -> int;
};

}
#endif //LIB_NINJA_SYNTAX_INCLUDED
