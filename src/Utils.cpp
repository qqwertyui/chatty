#include "Utils.hpp"
#include <winsock2.h>

std::string Utils::get_last_error() {
  char *s = nullptr;
  FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                     FORMAT_MESSAGE_IGNORE_INSERTS,
                 nullptr, WSAGetLastError(),
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&s, 0,
                 nullptr);
  std::string result(s);
  LocalFree(s);
  return result;
}

std::string Utils::basename(const std::string &path, bool backslash) {
  std::string separator = (backslash) ? "\\" : "/";
  std::string::size_type index = path.find_last_of(separator);
  if (index == std::string::npos) {
    return path;
  }
  return path.substr(index + 1);
}