#include "Message.hpp"
#include "Utils.hpp"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <fmt/core.h>

Message::Message(std::vector<unsigned char> &data, Client *client) {
  Message::rstrip_newlines(data);
  if (Message::validate_message(data) == false) {
    this->message_format = "{}{}{}";
    return;
  }
  this->valid = true;

  std::string line(data.begin(), data.end());
  if (line.front() == '/') {
    this->command = true;
  }
  this->content = line;

  using namespace std::chrono;
  std::time_t time = system_clock::to_time_t(system_clock::now());

  std::string s_time(30, 0);
  std::strftime(s_time.data(), s_time.size(), "%Y-%m-%d %H:%M:%S",
                std::localtime(&time));

  this->date = std::string(s_time.c_str());
  this->author = (client) ? client->get_username() : "*";
}

Message::Message(const std::string &message, Client *client)
    : Message(Utils::unmove<std::vector<unsigned char>>(
                  std::vector<unsigned char>(message.begin(), message.end())),
              client) {}

std::unique_ptr<Message> Message::create(std::string msg, Client *client) {
  return std::make_unique<Message>(msg, client);
}

void Message::set_format(const std::string &format) {
  this->message_format = format;
}

std::string Message::get_all() const {
  return fmt::format(this->message_format, this->date, this->author,
                     this->content);
}

std::string Message::get_content() const { return this->content; }

bool Message::is_command() const { return this->command; }

bool Message::is_valid() const { return this->valid; }

void Message::rstrip_newlines(std::vector<unsigned char> &container) {
  while (container.back() == '\n') {
    container.pop_back();
  }
}

template <typename T> bool Message::validate_message(const T &container) {
  return std::all_of(container.begin(), container.end(), isprint);
}