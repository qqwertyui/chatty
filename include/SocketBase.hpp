#ifndef SOCKETBASE_HPP
#define SOCKETBASE_HPP

#include <winsock2.h>
#include <string>

#ifndef TCP_KEEPIDLE
  #define TCP_KEEPIDLE 4
#endif

#ifndef TCP_KEEPINTVL
  #define TCP_KEEPINTVL 5
#endif

#ifndef TCP_KEEPCNT
  #define TCP_KEEPCNT 6
#endif

class NodeInfo {
public:
  NodeInfo(const std::string &ip, unsigned short port, int fd);
  NodeInfo() = default;

  std::string ip;
  unsigned short port;
  int fd;
};

class SocketBase {
public:
  NodeInfo get_nodeinfo();

protected:
  //SocketBase()  // move socket creation here
  NodeInfo ni;
};

#endif