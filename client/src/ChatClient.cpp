#include "ChatClient.hpp"
#include <cstdio>
#include <thread>

ChatClient::ChatClient(const std::string &ip) {
  initscr();
  this->output_window = newwin(LINES - 2, COLS, 0, 0);
  this->input_window = newwin(1, COLS, LINES - 1, 0);
  scrollok(this->output_window, TRUE);
  scrollok(this->input_window, TRUE);

  this->client =
      std::make_unique<TCPSocketWrapper>(ip, ChatClient::DEFAULT_CHAT_PORT);
}

ChatClient::~ChatClient() {
  if (this->is_connected()) {
    this->disconnect();
  }
  if (this->input_window) {
    delwin(this->input_window);
  }
  if (this->output_window) {
    delwin(this->output_window);
  }
  endwin();
}

bool ChatClient::is_connected() const {
  if (this->client) {
    return this->client->is_connected();
  }
  return false;
}

int ChatClient::connect(const std::string &username) {
  this->client->connect();
  ChatBase::HelloPacket packet = {ChatClient::VERSION,
                                  (unsigned char)username.size()};
  this->client->send((unsigned char *)&packet, sizeof(ChatBase::HelloPacket));
  this->client->send((unsigned char *)username.data(), username.size());

  std::vector<unsigned char> response =
      this->client->recieve(sizeof(ChatBase::StatusPacket));
  uint8_t status = (uint8_t)response[0];

  if (status != ChatClient::Status::SUCCESS) {
    return status;
  }

  std::thread msg_handler(
      (connectionHandler)&ChatClient::incoming_message_handler, this,
      this->client.get());
  msg_handler.detach();
  return ChatClient::Status::SUCCESS;
}

void ChatClient::disconnect() {
  this->client->send((unsigned char *)ChatClient::Command::EXIT.data(),
                     ChatClient::Command::EXIT.size());
  this->client->disconnect();
}

void ChatClient::send_message(const std::string &message) {
  if (this->is_connected() == false) {
    throw std::runtime_error("You are not connected");
  }
  if (message.compare(ChatClient::Command::EXIT) == 0) {
    this->disconnect();
    return;
  }
  this->client->send((unsigned char *)message.data(), message.size());
}

void ChatClient::incoming_message_handler(TCPSocketWrapper *client) {
  std::vector<unsigned char> data;
  while (this->is_connected()) {
    try {
      data = client->recieve(ChatClient::MAX_MESSAGE_LENGTH);
      data.push_back(0);
    } catch (const std::exception &e) {
      if (this->is_connected()) {
        this->disconnect();
        fputs("Message handler connection error", stderr);
      }
      return;
    }
    // mutex before write???
    wprintw(this->output_window, "%s\n", (const char *)data.data());
    wrefresh(this->output_window);
    wrefresh(
        this->input_window); // refresh so the cursor appears in input_window
  }
}

void ChatClient::interact() {
  char input[1024] = {0};
  wprintw(this->input_window, ChatClient::Interface::PROMPT.data());
  if (wgetstr(this->input_window, input) == ERR) {
    throw std::runtime_error("Input error");
  }
  this->send_message(input);
  wdeleteln(this->input_window);
  wmove(this->input_window, 0,0);
}