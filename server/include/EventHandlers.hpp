#ifndef EVENTHANDLERS_HPP
#define EVENTHANDLERS_HPP

#include <vector>
#include <string>

class ChatServer;
class Client;

class DefaultHandler {
public:
  static void client_connected(ChatServer *ctx, Client *client);
  static void invalid_characters(ChatServer *ctx, Client *client);

  class Command {
  public:
    static void help(ChatServer *ctx, Client *invoker, std::vector<std::string> &args);
    static void online(ChatServer *ctx, Client *invoker, std::vector<std::string> &args);
    static void exit(ChatServer *ctx, Client *invoker, std::vector<std::string> &args);
    static void msg(ChatServer *ctx, Client *invoker, std::vector<std::string> &args);
    static void unknown(Client *invoker);
  };
};

#endif