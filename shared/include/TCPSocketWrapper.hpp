#ifndef TCPSOCKETWRAPPER_HPP
#define TCPSOCKETWRAPPER_HPP

#include <stdexcept>
#include <string>
#include <vector>


class NodeInfo {
public:
  NodeInfo(const std::string &ip, unsigned short port, int fd);
  NodeInfo() = default;

  std::string ip;
  unsigned short port;
  int fd;
};

class TCPSocketWrapper {
public:
  TCPSocketWrapper(const std::string &ip, unsigned short port);
  explicit TCPSocketWrapper(const NodeInfo &ni);
  ~TCPSocketWrapper();

  void connect();
  void disconnect();
  bool is_connected() const;

  int send(std::vector<unsigned char> &data);
  int send(unsigned char *buffer, unsigned int size);
  std::vector<unsigned char> recieve(int max);

  // returns true if option is successfully set
  template <class T> bool set_option(int level, int option, T value);
  NodeInfo get_nodeinfo() const;

protected:
  static std::string get_last_error();

  NodeInfo ni;
  bool connected = false;
};

#endif