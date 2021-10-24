#include "Client.hpp"
#include <stdexcept>

Client::Client(const NodeInfo &ni, const std::string &username)
    : connection(ni), username(username) {}

Client::~Client() { this->disconnect(); }

unsigned int Client::send(const std::vector<unsigned char> &buffer) {
  int bytes_sent = ::send(this->connection.fd, (const char *)buffer.data(),
                          buffer.size(), 0);
  if (bytes_sent == SOCKET_ERROR) {
    throw std::runtime_error("Client send error");
  }
  return bytes_sent;
}

unsigned int Client::send(const std::string &message) {
  std::vector<unsigned char> data(message.begin(), message.end());
  return this->send(data);
}

std::vector<unsigned char> Client::recieve(unsigned int max) {
  std::vector<unsigned char> data(max);
  int bytes_recieved =
      recv(this->connection.fd, (char *)data.data(), data.size(), 0);
  if (bytes_recieved == SOCKET_ERROR) {
    throw std::runtime_error("Client recieve error");
  }
  if (bytes_recieved != (int)max) {
    std::vector<unsigned char> tmp(bytes_recieved);
    memcpy(tmp.data(), data.data(), bytes_recieved);
    return tmp;
  }
  return data;
}

void Client::disconnect() {
  closesocket(this->connection.fd);
  this->connected = false;
}

NodeInfo Client::get_nodeinfo() const { return this->connection; }

std::string Client::get_username() const { return this->username; }

bool Client::is_connected() const { return this->connected; }
