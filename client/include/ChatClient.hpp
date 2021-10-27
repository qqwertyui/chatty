#ifndef CHATCLIENT_HPP
#define CHATCLIENT_HPP

#include "ChatBase.hpp"
#include "TCPSocketWrapper.hpp"

#include <curses.h>
#include <memory>
#include <string_view>

typedef void (*connectionHandler)(void *, TCPSocketWrapper *);

class ChatClient : public ChatBase {
public:
  ChatClient(const std::string &ip);
  ~ChatClient();

  void interact();
  int connect(const std::string &username);
  void disconnect();

  bool is_connected() const;

private:
  std::unique_ptr<TCPSocketWrapper> client = nullptr;
  WINDOW *input_window = nullptr;
  WINDOW *output_window = nullptr;

  void incoming_message_handler(TCPSocketWrapper *client);
  void send_message(const std::string &message);

  struct Interface {
    static constexpr std::string_view PROMPT = "say: ";
  };
  struct Command {
    static constexpr std::string_view EXIT = "/exit";
  };
};

#endif