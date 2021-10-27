#include "ChatClient.hpp"
#include <spdlog/spdlog.h>

namespace Status {
constexpr int SUCCESS = 0;
constexpr int CMD_ARGS_ERROR = 1;
constexpr int USERNAME_USED = 2;
constexpr int CLIENT_ERROR = 3;
constexpr int USERNAME_TOO_LONG = 4;
constexpr int FORBIDDEN_CHARACTERS = 5;
constexpr int INVALID_VERSION = 6;
constexpr int UNKNOWN_ERROR = 7;
constexpr int CONNECTION_ERROR = 8;
}; // namespace Status

int main(int argc, char **argv) {
  spdlog::set_pattern("[%^%l%$] %v");
  const char *ip, *username;
  if (argc != 3) {
    spdlog::error("Usage: ./client <ip> <username>");
    return Status::CMD_ARGS_ERROR;
  }
  ip = argv[1];
  username = argv[2];

  ChatClient *client = new ChatClient(ip);
  int status;
  try {
    status = client->connect(username);
  } catch (const std::exception &e) {
    spdlog::error("Connection error: {}", e.what());
    return Status::CONNECTION_ERROR;
  }

  if (status == ChatClient::Status::USERNAME_IS_USED) {
    spdlog::error("This username is already used");
    return Status::USERNAME_USED;
  } else if (status == ChatClient::Status::USERNAME_TOO_LONG) {
    spdlog::error("This username is too long (max {} characters)",
                  ChatClient::MAX_NAME_LENGTH);
    return Status::USERNAME_TOO_LONG;
  } else if (status == ChatClient::Status::FORBIDDEN_CHARACTERS) {
    spdlog::error("Username cannot contain non-ascii characters");
    return Status::FORBIDDEN_CHARACTERS;
  } else if (status == ChatClient::Status::INVALID_VERSION) {
    spdlog::error("Invalid client version");
    return Status::INVALID_VERSION;
  } else if (status != ChatClient::Status::SUCCESS) {
    spdlog::error("Unknown error");
    return Status::UNKNOWN_ERROR;
  }

  while (client->is_connected()) {
    try {
      client->interact();
    } catch (const std::exception &e) {
      client->disconnect();
      spdlog::error("Client error: {}", e.what());
      return Status::CLIENT_ERROR;
    }
  }
  return Status::SUCCESS;
}