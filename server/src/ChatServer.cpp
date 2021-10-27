#include "ChatServer.hpp"

#include <algorithm>
#include <memory>
#include <spdlog/spdlog.h>

ChatServer::ChatServer(const std::string &ip) {
  this->server = new TCPSocketServer(ip, ChatBase::DEFAULT_CHAT_PORT);

  this->server->set_option<bool>(SOL_SOCKET, SO_REUSEADDR, true);
  this->server->set_option<int>(SOL_SOCKET, SO_KEEPALIVE,
                                ChatServer::KEEPALIVE_ACTIVE);
  this->server->set_option<int>(IPPROTO_TCP, TCP_KEEPIDLE,
                                ChatServer::KEEPALIVE_IDLE);
  this->server->set_option<int>(IPPROTO_TCP, TCP_KEEPINTVL,
                                ChatServer::KEEPALIVE_INTERVAL);
  this->server->set_option<int>(IPPROTO_TCP, TCP_KEEPCNT,
                                ChatServer::KEEPALIVE_MAX_PROBES);
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

Client *ChatServer::find_client_by_name(const std::string &name) {
  auto it = std::find_if(
      this->clients.begin(), this->clients.end(), [&name](Client *c) {
        return (c->get_username().compare(name) == 0) ? true : false;
      });
  return (it == this->clients.end()) ? nullptr : *it;
}

void ChatServer::handle_client(NodeInfo info) {
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
        client->recieve(ChatServer::MAX_MESSAGE_LENGTH);
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
  // TODO: try using std::map <std::string_view, commandHandler>

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
  std::unique_ptr<TCPSocketWrapper> conn =
      std::make_unique<TCPSocketWrapper>(ni);

  HelloPacket packet;
  std::vector<unsigned char> data = conn->recieve(sizeof(HelloPacket));
  memcpy(&packet, data.data(), data.size());

  if (packet.version != VERSION) {
    conn->send((unsigned char *)&ChatServer::Status::INVALID_VERSION,
               ChatServer::STATUS_SIZE);
    return nullptr;
  }
  if (packet.name_length > ChatBase::MAX_NAME_LENGTH) {
    conn->send((unsigned char *)&ChatServer::Status::USERNAME_TOO_LONG,
               ChatServer::STATUS_SIZE);
    return nullptr;
  }
  std::vector<unsigned char> name = conn->recieve(packet.name_length);

  bool printable = std::all_of(name.begin(), name.end(), isprint);
  if (printable == false) {
    conn->send((unsigned char *)&ChatServer::Status::FORBIDDEN_CHARACTERS,
               ChatServer::STATUS_SIZE);
    return nullptr;
  }

  std::string username(name.begin(), name.end());
  Client *client = this->find_client_by_name(username);
  if (client) {
    conn->send((unsigned char *)&ChatServer::Status::USERNAME_IS_USED,
               ChatServer::STATUS_SIZE);
    return nullptr;
  }
  conn->send((unsigned char *)&ChatServer::Status::SUCCESS,
             ChatServer::STATUS_SIZE);
  return (new Client(conn.release(), username));
}