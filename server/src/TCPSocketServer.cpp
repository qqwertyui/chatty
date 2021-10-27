#include "TCPSocketServer.hpp"

#include <cstdio>
#include <thread>

TCPSocketServer::TCPSocketServer(const std::string &ip, unsigned short port)
    : TCPSocketWrapper(ip, port) {}

void TCPSocketServer::serve_forever(void *object, connectionCallback handler) {
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(this->ni.port);
  addr.sin_addr.s_addr = inet_addr(this->ni.ip.c_str());

  int status = bind(this->ni.fd, (const sockaddr *)&addr, sizeof(addr));
  if (status == SOCKET_ERROR) {
    throw std::runtime_error(TCPSocketServer::get_last_error());
  }
  status = listen(this->ni.fd, TCPSocketServer::BACKLOG);
  if (status != 0) {
    throw std::runtime_error(TCPSocketServer::get_last_error());
  }

  while (true) {
    sockaddr_in client_info;
    socklen_t addrlen = sizeof(sockaddr_in);
    SOCKET client_fd =
        accept(this->ni.fd, (sockaddr *)&client_info, &addrlen);
    if (client_fd == INVALID_SOCKET) {
      printf("Couldn't accept connection: %s",
             TCPSocketServer::get_last_error().c_str());
      continue;
    }

    NodeInfo client(inet_ntoa(client_info.sin_addr),
                    htons(client_info.sin_port), client_fd);
    std::thread t(handler, object, client);
    t.detach();
  }
}

void TCPSocketServer::shutdown() {
  (dynamic_cast<TCPSocketWrapper *>(this))->disconnect();
}