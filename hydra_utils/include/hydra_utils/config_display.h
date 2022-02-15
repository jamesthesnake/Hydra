#pragma once
#include "hydra_utils/config_traits.h"

#include <map>
#include <ostream>
#include <vector>

namespace config_parser {

namespace detail {

template <typename T>
void displayParam(std::ostream& out, const T& value) {
  out << value;
}

void displayParam(std::ostream& out, const uint8_t& value) {
  out << static_cast<int>(value);
}

void displayParam(std::ostream& out, const bool& value) {
  out << (value ? "true" : "false");
}

// adl indirection
struct display_param_fn {
  template <typename T>
  constexpr auto operator()(std::ostream& out, const T& value) const
      -> decltype(displayParam(out, value)) {
    return displayParam(out, value);
  }
};

}  // namespace detail

namespace {

constexpr const auto& displayParam = detail::static_const<detail::display_param_fn>;

}  // namespace

// make sure vector operator is present in the right namespace
template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& values) {
  out << "[";
  auto iter = values.begin();
  while (iter != values.end()) {
    out << *iter;
    ++iter;
    if (iter != values.end()) {
      out << ", ";
    }
  }
  out << "]";
  return out;
}

template <typename K, typename V>
std::ostream& operator<<(std::ostream& out, const std::map<K, V>& values) {
  out << "{";
  auto iter = values.begin();
  while (iter != values.end()) {
    out << iter->first << ": " << iter->second;
    ++iter;
    if (iter != values.end()) {
      out << ", ";
    }
  }
  out << "}";
  return out;
}

class ConfigDisplay {
 public:
  explicit ConfigDisplay(std::ostream& out)
      : out_(out), prefix_(""), root_call_(true) {}

  ConfigDisplay operator[](const std::string& name) const {
    std::string new_prefix = "";

    const auto prev_pos = prefix_.find("-");
    if (prev_pos == std::string::npos) {
      new_prefix = "- " + name + ":";
    } else if (prev_pos == 0) {
      new_prefix = "  - " + name + ":";
    } else {
      new_prefix = "  " + prefix_.substr(prev_pos) + "- " + name + ":";
    }

    return ConfigDisplay(out_, new_prefix);
  }

  void pre_visit() const {
    if (!root_call_) {
      out_ << prefix_;
    }
  }

  void post_visit() const {
    if (!root_call_) {
      out_ << std::endl;
    }
  }

  template <typename T>
  void visit(const T& value) const {
    out_ << " ";
    displayParam(out_, value);
  }

 private:
  ConfigDisplay(std::ostream& out, const std::string& prefix)
      : out_(out), prefix_(prefix), root_call_(false) {}

  std::ostream& out_;
  std::string prefix_;
  bool root_call_;
};

}  // namespace config_parser
