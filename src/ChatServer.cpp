#include "ChatServer.hpp"

#include <algorithm>
#include <csignal>
#include <fmt/core.h>
#include <memory>
#include <spdlog/spdlog.h>
#include <winsock2.h>

ChatServer::ChatServer(const std::string &ip) {
  this->server = new SocketServer(ip, ChatBase::DEFAULT_CHAT_PORT);
  bool reuseaddr = true;
  setsockopt(this->server->get_nodeinfo().fd, SOL_SOCKET, SO_REUSEADDR,
             (char *)&reuseaddr, sizeof(reuseaddr));

  std::signal(SIGINT, ChatServer::ctrl_c_handler);
}

void ChatServer::run() {
  this->server->serve_forever(this,
                              (connectionCallback)&ChatServer::handle_client);
}

ChatServer::~ChatServer() {
  if (this->server) {
    for (Client *client : this->clients) {
      delete client;
    }
  }
}

void ChatServer::add_client(Client *client) {
  this->add_client_mutex.lock();
  this->clients.push_back(client);
  this->add_client_mutex.unlock();
}

void ChatServer::remove_client(Client *client) {
  this->remove_client_mutex.lock();
  this->clients.erase(
      std::find(this->clients.begin(), this->clients.end(), client));
  this->remove_client_mutex.unlock();
}

void ChatServer::handle_client(NodeInfo info) {
  setsockopt(info.fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&ChatServer::TIMEOUT,
             sizeof(ChatServer::TIMEOUT));

  Client *client = this->handle_handshake(info);
  if (client == nullptr) {
    return;
  }
  spdlog::info(fmt::format("{} connected", client->get_username()));
  this->broadcast_message(
      Message::create(fmt::format("{} connected", client->get_username()))
          .get());
  this->add_client(client);

  client->send(fmt::format("Welcome {}, there are {} users online!",
                           client->get_username(), this->clients.size()));

  try {
    this->client_loop(client);
  } catch (const std::exception &e) {
    spdlog::error("Client error: {}", e.what());
  }

  this->remove_client(client);
  this->broadcast_message(
      Message::create(fmt::format("{} disconnected", client->get_username()))
          .get());
  spdlog::info(fmt::format("{} disconnected", client->get_username()));
}

void ChatServer::client_loop(Client *client) {
  do {
    std::vector<unsigned char> data =
        client->recieve(ChatBase::MAX_LINE_LENGTH);
    std::unique_ptr<Message> msg = std::make_unique<Message>(data, client);
    if (msg->is_valid() == false) {
      client->send("[!] Only ASCII characters are allowed");
      continue;
    }

    if (msg->is_command()) {
      this->execute_command(client, msg.get());
    } else {
      this->broadcast_message(msg.get());
    }
  } while (client->is_connected());
}

void ChatServer::broadcast_message(Message *message) {
  std::lock_guard<std::mutex> guard(this->broadcast_mutex);
  for (Client *client : this->clients) {
    client->send(message->get_all());
  }
}

void ChatServer::execute_command(Client *client, Message *message) {
  std::lock_guard<std::mutex> guard(this->command_mutex);
  std::string cmd = message->get_content();
  // maybe use std::map <std::string_view, commandHandler>

  if (cmd.compare("/online") == 0) {
    std::string online;
    for_each(this->clients.begin(), this->clients.end(),
             [&online](Client *client) {
               online.append(client->get_username()).append(" ");
             });
    client->send(online);
  } else if (cmd.compare("/exit") == 0) {
    client->disconnect();
  } else if (cmd.compare("/help") == 0) {
    std::string help_message = "Possible commands are: ";
    for_each(ChatServer::COMMANDS.begin(), ChatServer::COMMANDS.end(),
             [&help_message](const std::string_view &str) {
               help_message.append(str).append("\n");
             });
    if (help_message.back() == '\n') {
      help_message.pop_back();
    }
    client->send(help_message);
  }
  /*
  else if(cmd.compare("/msg")) {

  }
  */
  else {
    client->send("Unrecognized command");
  }
}

Client *ChatServer::handle_handshake(NodeInfo ni) {
  HelloPacket packet;
  int bytes_transferred = recv(ni.fd, (char *)&packet, sizeof(HelloPacket), 0);
  if (bytes_transferred == SOCKET_ERROR) {
    closesocket(ni.fd);
    return nullptr;
  }
  if (packet.version != VERSION) {
    send(ni.fd, (char *)&ChatServer::Status::INVALID_VERSION,
         ChatServer::STATUS_SIZE, 0);
    closesocket(ni.fd);
    return nullptr;
  }
  if (packet.name_length > ChatBase::MAX_NAME_LENGTH) {
    send(ni.fd, (char *)&ChatServer::Status::USERNAME_TOO_LONG,
         ChatServer::STATUS_SIZE, 0);
    closesocket(ni.fd);
    return nullptr;
  }
  std::vector<char> name(packet.name_length +
                         2); // newline + null character (TODO: fix it)
  memset(name.data(), 0, name.size());
  bytes_transferred = recv(ni.fd, name.data(), name.size(), 0);
  if (bytes_transferred == SOCKET_ERROR) {
    closesocket(ni.fd);
    return nullptr;
  }
  bool printable = std::all_of(name.begin(), name.end() - 2, isprint);
  if (printable == false) {
    send(ni.fd, (char *)&ChatServer::Status::FORBIDDEN_CHARACTERS,
         ChatServer::STATUS_SIZE, 0);
    closesocket(ni.fd);
    return nullptr;
  }

  std::string username(name.begin(), name.end() - 2);
  bool nickname_used =
      std::any_of(clients.begin(), clients.end(), [&username](Client *c) {
        return (c->get_username().compare(username) == 0) ? true : false;
      });

  if (nickname_used) {
    send(ni.fd, (char *)&ChatServer::Status::USERNAME_IS_USED,
         ChatServer::STATUS_SIZE, 0);
    closesocket(ni.fd);
    return nullptr;
  }
  send(ni.fd, (char *)&ChatServer::Status::OK, ChatServer::STATUS_SIZE, 0);
  return (new Client(ni, username));
}

void ChatServer::ctrl_c_handler(int) {
  spdlog::info("User requested program termination, but doing nothing");
}