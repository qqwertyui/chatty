#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include "Client.hpp"

#include <memory>
#include <string_view>
#include <vector>

class Message {
public:
  Message(std::vector<unsigned char> &data, Client *client = nullptr);
  Message(const std::string &message, Client *client = nullptr);

  static std::unique_ptr<Message> create(std::string msg,
                                         Client *client = nullptr);

  std::string get_all() const;
  std::string get_content() const;
  bool is_command() const;
  bool is_valid() const;
  void set_format(const std::string &format);

  static constexpr std::string_view DEFAULT_FORMAT = "[{} {}] {}";
  static constexpr Client *SYSTEM = nullptr;

private:
  template <typename T> bool validate_message(const T &container);
  void rstrip_newlines(std::vector<unsigned char> &container);

  std::string date;
  std::string author;
  std::string content;

  bool command = false;
  bool valid = false;
  std::string_view message_format = DEFAULT_FORMAT;
};

#endif