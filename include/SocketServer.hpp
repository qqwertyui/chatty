#ifndef SOCKETSERVER_HPP
#define SOCKETSERVER_HPP

#include "SocketBase.hpp"

typedef void (*connectionCallback)(void *object, NodeInfo ni);

class SocketServer : public SocketBase {
public:
  SocketServer(const std::string &ip, unsigned short port);
  ~SocketServer();
  void serve_forever(void *object, connectionCallback handler);

private:
  static constexpr int BACKLOG = 32;
};

#endif