#ifndef CHATSERVER_HPP
#define CHATSERVER_HPP

#include "ChatBase.hpp"
#include "Client.hpp"
#include "SocketServer.hpp"
#include "Message.hpp"

#include <mutex>
#include <vector>
#include <array>
#include <string_view>

class ChatServer : public ChatBase {
public:
  ChatServer(const std::string &ip);
  ~ChatServer();

  void run();

private:
  void handle_client(NodeInfo info);
  Client* handle_handshake(NodeInfo info);

  void execute_command(Client *client, Message *message);
  void broadcast_message(Message *message);

  static void ctrl_c_handler(int signal);

  static constexpr std::string_view WELCOME = "Connected to test chat!";
  static constexpr std::array<std::string_view, 3> COMMANDS = {
    "/help", "/online", "/exit"
  };

  void client_loop(Client *current);
  void add_client(Client *client);
  void remove_client(Client *client);

  std::vector<Client*> clients;
  std::mutex broadcast_mutex, remove_client_mutex, add_client_mutex, command_mutex;

  SocketServer *server = nullptr;
};

/*
  static constexpr const char *INVALID_VERSION = "Invalid client version!";
  static constexpr const char *USERNAME_TOO_LONG = "Username is too long!";
  static constexpr const char *INVALID_CHARS = "Invalid characters in nickname!";
*/

#endif