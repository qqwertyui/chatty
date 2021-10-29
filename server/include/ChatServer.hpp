#ifndef CHATSERVER_HPP
#define CHATSERVER_HPP

#include "ChatBase.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "TCPSocketServer.hpp"
#include "EventHandlers.hpp"

#include <array>
#include <mutex>
#include <string_view>
#include <vector>
#include <map>

class ChatServer : public ChatBase {
public:
  ChatServer(const std::string &ip);
  ~ChatServer();

  void run();

  typedef void (*commandHandler)(ChatServer *ctx, Client *invoker, 
  std::vector<std::string> &args);
  void register_command(const std::string &str, commandHandler handler);

private:
  void handle_client(NodeInfo info);
  Client *handle_handshake(NodeInfo info);

  void execute_command(Client *client, Message *message);
  void broadcast_message(Message *message);

  static void ctrl_c_handler(int signal);

  static constexpr int KEEPALIVE_ACTIVE = 1;
  static constexpr int KEEPALIVE_IDLE = 30;
  static constexpr int KEEPALIVE_INTERVAL = 5;
  static constexpr int KEEPALIVE_MAX_PROBES = 5;

  void client_loop(Client *current);
  void add_client(Client *client);
  void remove_client(Client *client);
  Client *find_client_by_name(const std::string &name);

  std::map<std::string, commandHandler> commands;
  std::vector<Client*> clients;
  std::mutex broadcast_mutex, remove_client_mutex, add_client_mutex,
      command_mutex;

  TCPSocketServer *server = nullptr;
  friend class DefaultHandler;
};

#endif