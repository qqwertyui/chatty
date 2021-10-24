#ifndef SOCKETBASE_HPP
#define SOCKETBASE_HPP

#include <winsock2.h>
#include <string>

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