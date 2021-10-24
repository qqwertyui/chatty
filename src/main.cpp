#include "ChatServer.hpp"
#include "Utils.hpp"
#include <spdlog/spdlog.h>

namespace Status {
constexpr unsigned int OK = 0;
constexpr unsigned int INVALID_ARGS = 1;
constexpr unsigned int INITIALIZATION_ERROR = 2;
constexpr unsigned int CONNECTION_ERROR = 3;
} // namespace Status

int main(int argc, char **argv) {
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
  if (argc < 2) {
    spdlog::error("Usage: {} <ip>", Utils::basename(argv[0]));
    return Status::INVALID_ARGS;
  }

  ChatServer *chat = nullptr;
  try {
    chat = new ChatServer(argv[1]);
  } catch (const std::exception &e) {
    spdlog::error("ChatServer error: {}", e.what());
    return Status::INITIALIZATION_ERROR;
  }

  try {
    spdlog::info("Waiting for clients...");
    chat->run();
  } catch (const std::exception &e) {
    spdlog::error("Critical chat error: {}", e.what());
    return Status::CONNECTION_ERROR;
  }

  spdlog::info("Stopping server...");
  return Status::OK;
}