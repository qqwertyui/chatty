#include "SocketServer.hpp"
#include "Utils.hpp"
#include <cstdio>
#include <stdexcept>
#include <thread>

NodeInfo::NodeInfo(const std::string &ip, unsigned short port, int fd)
    : ip(ip), port(port), fd(fd) {}

SocketServer::SocketServer(const std::string &ip, unsigned short port) {
  this->ni.ip = ip;
  this->ni.port = port;

  WSADATA wsaData;
  int status = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (status != 0) {
    throw std::runtime_error(Utils::get_last_error());
  }

  this->ni.fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (this->ni.fd == INVALID_SOCKET) {
    throw std::runtime_error(Utils::get_last_error());
  }

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip.c_str());

  status = bind(this->ni.fd, (const sockaddr *)&addr, sizeof(addr));
  if (status == SOCKET_ERROR) {
    throw std::runtime_error(Utils::get_last_error());
  }
}

SocketServer::~SocketServer() {
  closesocket(this->ni.fd);
  WSACleanup();
}

void SocketServer::serve_forever(void *object, connectionCallback handler) {
  int status = listen(this->ni.fd, SocketServer::BACKLOG);
  if (status != 0) {
    throw std::runtime_error(Utils::get_last_error());
  }

  while (true) {
    sockaddr_in client_info;
    int addrlen = sizeof(sockaddr_in);
    int client_fd = accept(this->ni.fd, (sockaddr *)&client_info, &addrlen);
    if (client_fd == INVALID_SOCKET) {
      printf("Couldn't accept connection: %s", Utils::get_last_error().c_str());
      continue;
    }

    NodeInfo client(inet_ntoa(client_info.sin_addr),
                    htons(client_info.sin_port), client_fd);
    std::thread t(handler, object, client);
    t.detach();
  }
}