#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "TCPSocketWrapper.hpp"
#include <vector>

class Client {
public:
  explicit Client(TCPSocketWrapper *conn, const std::string &username);
  ~Client();

  std::string get_username() const;
  bool is_connected() const;

  unsigned int send(std::vector<unsigned char> &buffer);
  unsigned int send(const std::string &message);
  std::vector<unsigned char> recieve(unsigned int max);
  void disconnect();

private:
  TCPSocketWrapper *conn = nullptr;
  std::string username;
};

#endif