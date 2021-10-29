#include "Utils.hpp"
#include <vector>
#include <sstream>

std::string Utils::basename(const std::string &path, bool backslash) {
  std::string separator = (backslash) ? "\\" : "/";
  std::string::size_type index = path.find_last_of(separator);
  if (index == std::string::npos) {
    return path;
  }
  return path.substr(index + 1);
}

std::vector<std::string> Utils::split(std::string &text, char delimiter) {
  std::vector<std::string> result;
  std::istringstream iss(text);

  std::string line;
  while(std::getline(iss, line, delimiter)) {
    result.push_back(line);
  }
  return result;
}

template <class T> T &Utils::unmove(T &&t) { return t; }
template std::vector<unsigned char> &
Utils::unmove(std::vector<unsigned char> &&t);