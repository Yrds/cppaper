#include "ninja_syntax.hpp"
#include <ranges>
#include <iostream>

namespace ninja_lib {

auto Writer::create(
  std::unique_ptr<std::ostream> stream,
  uint8_t indent_size
) -> std::expected<Writer, WriterError> {
  try {
    Writer writer;
    writer.stream = std::move(stream);
    writer.indent_size = indent_size;
    return writer;
  } catch(...) {
    return std::unexpected(WriterError::kUnknownError);
  }
}

auto Writer::new_line(int count) const -> void {
  std::cout << "Adding " << count << " new lines\n";
  for (int i = 0; i < count; ++i) {
    (*stream) << '\n';
  }
}

auto Writer::comment(std::string_view comment_text) const -> void {
  for (auto line: std::views::split(comment_text, '\n')) {
    (*stream) << "# " << std::string_view(line) << '\n';
  }
}

auto Writer::variable(std::string_view key) const -> void {
  (*stream) << key << " =\n";
}

auto Writer::variable(std::string_view key, std::string_view value, int indent) const -> void {
  std::string indent_str(static_cast<uint>(indent * indent_size), ' ');
  (*stream) << indent_str << key << " = " << value << '\n';
}

auto Writer::variable(std::string_view key, std::vector<std::string_view>& values, int indent) const -> void {
  std::string indent_str(static_cast<uint>(indent * indent_size), ' ');
  (*stream) << indent_str << key << " =";
  for (const auto& value : values) {
    (*stream) << " " << value;
  }
  (*stream) << '\n';
}


auto Writer::build(
  const std::vector<std::string_view>& outputs,
  std::string_view rule,
  const std::vector<std::string_view>& inputs,
  const std::vector<std::pair<std::string_view, std::string_view>>& variables,
  const std::vector<std::string_view>& implicit,
  std::string_view pool,
  const std::vector<std::string_view>& implicit_outputs,
  std::string_view dyndep
) const -> void {
  (*stream) << "build ";
  for (const auto& output : outputs) {
    (*stream) << output << " ";
  }
  if (!implicit_outputs.empty()) {
    (*stream) << "| ";
    for (const auto& implicit_output : implicit_outputs) {
      (*stream) << implicit_output << " ";
    }
  }
  (*stream) << ": " << rule << " ";
  for (const auto& input : inputs) {
    (*stream) << input << " ";
  }
  if (!implicit.empty()) {
    (*stream) << "| ";
    for (const auto& impl : implicit) {
      (*stream) << impl << " ";
    }
  }
  (*stream) << '\n';

  for (const auto& var_pair : variables) {
    (*stream) << "  " << var_pair.first << " = " << var_pair.second << '\n';
  }

  if (!pool.empty()) {
    (*stream) << "  pool = " << pool << '\n';
  }

  if (!dyndep.empty()) {
    (*stream) << "  dyndep = " << dyndep << '\n';
  }
}



} // namespace ninja_lib
