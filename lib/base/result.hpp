#pragma once
#include <utility>
#include <string>
#include <optional>

namespace mm {

template <typename T>
class Result {
 public:
  static Result ok(T v) { return Result(std::move(v), std::nullopt); }
  static Result err(std::string e) { return Result(std::nullopt, std::move(e)); }

  bool has_value() const { return value_.has_value(); }
  const T& value() const { return *value_; }
  T& value() { return *value_; }
  const std::string& error() const { return *error_; }

 private:
  Result(std::optional<T> v, std::optional<std::string> e) : value_(std::move(v)), error_(std::move(e)) {}
  std::optional<T> value_;
  std::optional<std::string> error_;
};

} // namespace mm
