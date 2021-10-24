#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

namespace Utils {
  std::string get_last_error();
  std::string basename(const std::string &path, bool backslash = true);
}

#endif
