#include "TCPSocketWrapper.hpp"

NodeInfo::NodeInfo(const std::string &ip, unsigned short port, SOCKET fd)
    : ip(ip), port(port), fd(fd) {}

#ifdef _WIN32
  bool TCPSocketWrapper::initialized = false;
#elif __linux__ // linux doesn't need explicit initialization
  bool TCPSocketWrapper::initialized = true;
#endif

TCPSocketWrapper::TCPSocketWrapper(const std::string &ip, unsigned short port) {
  TCPSocketWrapper::initialize();
  SOCKET fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (fd == INVALID_SOCKET) {
    throw std::runtime_error(TCPSocketWrapper::get_last_error());
  }
  this->ni = NodeInfo(TCPSocketWrapper::resolve_hostname(ip), port, fd);
}

TCPSocketWrapper::TCPSocketWrapper(const NodeInfo &ni) {
  TCPSocketWrapper::initialize();
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
  #ifdef _WIN32
    WSACleanup();
  #endif
}

bool TCPSocketWrapper::set_option(int level, int option, int value) {
  int status =
      setsockopt(this->ni.fd, level, option, (const char *)&value, sizeof(int));
  return (status == 0) ? true : false;
}

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
  std::string result;
  #ifdef _WIN32
    char *s = nullptr;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS,
                  nullptr, WSAGetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&s, 0,
                  nullptr);
    result = s;
    LocalFree(s);
  #elif __linux__
    result = strerror(errno);
  #endif
  return result;
}

std::string TCPSocketWrapper::resolve_hostname(const std::string &hostname) {
  TCPSocketWrapper::initialize();
  hostent *ent = gethostbyname(hostname.c_str());
  if(ent == nullptr) {
    throw std::runtime_error("Couldn't resolve given hostname");
  }
  return inet_ntoa(*(in_addr*)ent->h_addr);
}

void TCPSocketWrapper::initialize() {
  if(TCPSocketWrapper::initialized) {
    return;
  }
  #ifdef _WIN32
    WSADATA wsaData;
    int error = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (error != 0) {
      throw std::runtime_error(TCPSocketWrapper::get_last_error());
    }
    TCPSocketWrapper::initialized = true;
  #endif
}

NodeInfo TCPSocketWrapper::get_nodeinfo() const { return this->ni; }

bool TCPSocketWrapper::is_connected() const { return this->connected; }