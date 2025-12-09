#ifndef CPPAPER_ARG_VALUE_CONSTRAINT_HPP_INCLUDED
#define CPPAPER_ARG_VALUE_CONSTRAINT_HPP_INCLUDED
#include "tclap/Constraint.h"
#include "tclap/SwitchArg.h"

template <typename T>
class ArgValueConstraint: public TCLAP::Constraint<T> {
  const TCLAP::SwitchArg& _arg;
  public:
    explicit ArgValueConstraint(const TCLAP::SwitchArg& arg): _arg(arg)  {}

    [[nodiscard]] auto description() const -> std::string override {
      return "Usable only with: -" + _arg.getFlag();
    };

    [[nodiscard]] auto shortID() const -> std::string override {
      return "(-" + _arg.getFlag() + ")";
    }

    [[nodiscard]] auto check(const T& /*value*/) const -> bool override {
      return _arg.isSet();
    }

    ~ArgValueConstraint() override = default;
};
// TODO(yuri): Implement ArgValueConstraint
//
//this will be a conditional constraint to validate a arg if other arg has an expected value
//
// like -C arg can be only used if a unlabeled arg have the value "build"
//
// Notes: this will probably be a template because we need to know the value of the arg to compare against
//
#endif // CPPAPER_ARG_VALUE_CONSTRAINT_HPP_INCLUDED
