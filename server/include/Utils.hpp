#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

namespace Utils {
std::string basename(const std::string &path, bool backslash = true);
template <class T> T &unmove(T &&t);
} // namespace Utils

#endif
