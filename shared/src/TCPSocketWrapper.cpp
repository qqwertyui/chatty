#include "TCPSocketWrapper.hpp"

#include <mstcpip.h>

NodeInfo::NodeInfo(const std::string &ip, unsigned short port, int fd)
    : ip(ip), port(port), fd(fd) {}

TCPSocketWrapper::TCPSocketWrapper(const std::string &ip, unsigned short port) {
  WSADATA wsaData;
  int error = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (error != 0) {
    throw std::runtime_error(TCPSocketWrapper::get_last_error());
  }
  unsigned int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (fd == INVALID_SOCKET) {
    throw std::runtime_error(TCPSocketWrapper::get_last_error());
  }
  this->ni = NodeInfo(ip, port, static_cast<int>(fd));
}

TCPSocketWrapper::TCPSocketWrapper(const NodeInfo &ni) {
  this->ni = ni;
  this->connected = true;
}

void TCPSocketWrapper::connect() {
  if (this->connected == true) {
    return;
  }
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(this->ni.port);
  addr.sin_addr.s_addr = inet_addr(this->ni.ip.c_str());

  int status = ::connect(this->ni.fd, (const sockaddr *)&addr, sizeof(addr));
  if (status == SOCKET_ERROR) {
    throw std::runtime_error(TCPSocketWrapper::get_last_error());
  }
  this->connected = true;
}

void TCPSocketWrapper::disconnect() {
  char buffer[1024];
  shutdown(this->ni.fd, SD_SEND);
  while (recv(this->ni.fd, buffer, 1024, 0) > 0) {
  }
  closesocket(this->ni.fd);
  this->connected = false;
}

TCPSocketWrapper::~TCPSocketWrapper() {
  if (this->connected) {
    this->disconnect();
  }
  WSACleanup();
}

template <typename T>
bool TCPSocketWrapper::set_option(int level, int option, T value) {
  int status =
      setsockopt(this->ni.fd, level, option, (const char *)&value, sizeof(T));
  return (status == 0) ? true : false;
}
template bool TCPSocketWrapper::set_option<int>(int, int, int);
template bool TCPSocketWrapper::set_option<bool>(int, int, bool);

int TCPSocketWrapper::send(std::vector<unsigned char> &data) {
  int bytes_sent =
      ::send(this->ni.fd, (const char *)data.data(), data.size(), 0);
  if (bytes_sent == SOCKET_ERROR) {
    throw std::runtime_error(TCPSocketWrapper::get_last_error());
  }
  return bytes_sent;
}

int TCPSocketWrapper::send(unsigned char *buffer, unsigned int size) {
  int bytes_sent = ::send(this->ni.fd, (const char *)buffer, size, 0);
  if (bytes_sent == SOCKET_ERROR) {
    throw std::runtime_error(TCPSocketWrapper::get_last_error());
  }
  return bytes_sent;
}

std::vector<unsigned char> TCPSocketWrapper::recieve(int max) {
  std::vector<unsigned char> tmp(max);
  int bytes_recieved = recv(this->ni.fd, (char *)tmp.data(), max, 0);
  if (bytes_recieved == SOCKET_ERROR) {
    throw std::runtime_error(TCPSocketWrapper::get_last_error());
  }
  std::vector<unsigned char> result(bytes_recieved);
  memcpy(result.data(), tmp.data(), bytes_recieved);
  return result;
}

std::string TCPSocketWrapper::get_last_error() {
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

NodeInfo TCPSocketWrapper::get_nodeinfo() const { return this->ni; }

bool TCPSocketWrapper::is_connected() const { return this->connected; }