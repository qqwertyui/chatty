#include "EventHandlers.hpp"
#include "ChatServer.hpp"
#include <algorithm>
#include <fmt/core.h>

void DefaultHandler::client_connected(ChatServer *ctx, Client *client) {
  client->send(fmt::format("Welcome {}, there are {} users online!",
                           client->get_username(), ctx->clients.size()));
}

void DefaultHandler::invalid_characters(ChatServer*, Client *client) {
  client->send("Only ASCII characters are allowed");
}

void DefaultHandler::Command::online(ChatServer *ctx, Client *invoker, 
  std::vector<std::string>&) {
  std::string online;
  std::for_each(ctx->clients.begin(), ctx->clients.end(),
          [&online](Client *client) {
            online.append(client->get_username()).append(" ");
          });
  invoker->send(online);
}

void DefaultHandler::Command::exit(ChatServer*, Client *invoker, 
  std::vector<std::string>&) {
  invoker->disconnect();
}

void DefaultHandler::Command::help(ChatServer *ctx, Client *invoker, 
  std::vector<std::string>&) {
  std::string help_message = "Avalible commands are: ";
  for(auto const& v : ctx->commands) {
    help_message.append(v.first).append("\n");
  }
  if (help_message.back() == '\n') {
    help_message.pop_back();
  }
  invoker->send(help_message);
}

void DefaultHandler::Command::msg(ChatServer *ctx, Client *invoker, 
  std::vector<std::string> &args) {
  if(args.size() < 3) {
    invoker->send("Usage: /msg <user> <message>");
    return;
  }
  std::string to = args[1];
  std::string msg = args[2];

  Client *reciepent = ctx->find_client_by_name(to);
  if(reciepent == nullptr) {
    invoker->send("User not found");
    return;
  }
  if(msg.size() == 0) {
    invoker->send("Message cannot be empty");
    return;
  }
  reciepent->send(fmt::format("{} whispers: {}", invoker->get_username(), msg));
}

void DefaultHandler::Command::unknown(Client *invoker) {
  invoker->send("Unrecognized command");
}