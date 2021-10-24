#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "SocketBase.hpp"
#include <vector>

class Client {
public:
  Client(const NodeInfo &ni, const std::string &username);
  ~Client();
  NodeInfo get_nodeinfo() const;
  std::string get_username() const;
  bool is_connected() const;

  unsigned int send(const std::vector<unsigned char> &buffer);
  unsigned int send(const std::string &message);
  std::vector<unsigned char> recieve(unsigned int max);
  void disconnect();

private:
  NodeInfo connection;
  std::string username;
  bool connected = true;
};


#endif