#include "Client.hpp"
#include <stdexcept>

Client::Client(TCPSocketWrapper *conn, const std::string &username)
    : conn(conn), username(username) {}

Client::~Client() { this->conn->disconnect(); }

unsigned int Client::send(std::vector<unsigned char> &buffer) {
  return this->conn->send(buffer);
}

unsigned int Client::send(const std::string &message) {
  std::vector<unsigned char> data(message.begin(), message.end());
  return this->conn->send(data);
}

std::vector<unsigned char> Client::recieve(unsigned int max) {
  return this->conn->recieve(max);
}

void Client::disconnect() { this->conn->disconnect(); }

std::string Client::get_username() const { return this->username; }

bool Client::is_connected() const { return this->conn->is_connected(); }
