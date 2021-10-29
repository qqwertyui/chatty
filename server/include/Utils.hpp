#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

namespace Utils {
std::string basename(const std::string &path, bool backslash = true);
template <class T> T &unmove(T &&t);
std::vector<std::string> split(std::string &text, char delimiter);
} // namespace Utils

#endif
