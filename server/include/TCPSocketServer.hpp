#ifndef TCPSOCKETSERVER_HPP
#define TCPSOCKETSERVER_HPP

#include "TCPSocketWrapper.hpp"

#ifdef _WIN32
  #ifndef TCP_KEEPIDLE
    #define TCP_KEEPIDLE 4
  #endif
  #ifndef TCP_KEEPINTVL
    #define TCP_KEEPINTVL 5
  #endif
  #ifndef TCP_KEEPCNT
    #define TCP_KEEPCNT 6
  #endif
#elif __linux__
  #include <netinet/tcp.h>
#endif

typedef void (*connectionCallback)(void *object, NodeInfo ni);

class TCPSocketServer : public TCPSocketWrapper {
public:
  TCPSocketServer(const std::string &ip, unsigned short port);
  void serve_forever(void *object, connectionCallback handler);
  void shutdown();

  void connect() = delete;
  void disconnect() = delete;

private:
  static constexpr int BACKLOG = 32;
};

#endif