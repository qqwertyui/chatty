#ifndef TCPSOCKETWRAPPER_HPP
#define TCPSOCKETWRAPPER_HPP

#include <stdexcept>
#include <string>
#include <vector>

#ifdef _WIN32
  #include <winsock2.h>
  #include <mstcpip.h>
#elif __linux__
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <unistd.h>
  #include <sys/types.h>
  #include <arpa/inet.h>
  #include <ifaddrs.h>
  #include <errno.h>
  #include <cstring> // strerror

  #define SOCKET_ERROR -1
  #define INVALID_SOCKET -1
  #define SD_SEND SHUT_WR
  #define closesocket(x) close(x)

  typedef int SOCKET;
#endif

class NodeInfo {
public:
  NodeInfo(const std::string &ip, unsigned short port, SOCKET fd);
  NodeInfo() = default;

  std::string ip;
  unsigned short port;
  SOCKET fd;
};

class TCPSocketWrapper {
public:
  TCPSocketWrapper(const std::string &ip, unsigned short port);
  explicit TCPSocketWrapper(const NodeInfo &ni);
  ~TCPSocketWrapper();

  void connect();
  void disconnect();
  bool is_connected() const;

  int send(std::vector<unsigned char> &data);
  int send(unsigned char *buffer, unsigned int size);
  std::vector<unsigned char> recieve(int max);

  // returns true if option is successfully set
  bool set_option(int level, int option, int value);
  NodeInfo get_nodeinfo() const;

protected:
  static std::string get_last_error();

  NodeInfo ni;
  bool connected = false;
};

#endif