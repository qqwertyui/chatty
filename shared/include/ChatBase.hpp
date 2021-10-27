#ifndef CHATBASE_HPP
#define CHATBASE_HPP

#include <cstdint>
#include <string>

class ChatBase {
public:
  struct HelloPacket {
    uint8_t version;
    uint8_t name_length;
  } __attribute__((packed));

  struct StatusPacket {
    uint8_t code;
  } __attribute__((packed));

  struct Status {
    static constexpr uint8_t SUCCESS = 0;
    static constexpr uint8_t USERNAME_IS_USED = 1;
    static constexpr uint8_t USERNAME_TOO_LONG = 2;
    static constexpr uint8_t FORBIDDEN_CHARACTERS = 3;
    static constexpr uint8_t INVALID_VERSION = 4;
  };

  static constexpr unsigned short DEFAULT_CHAT_PORT = 5999;
  static constexpr unsigned int STATUS_SIZE = sizeof(StatusPacket);

  static constexpr uint8_t VERSION = 2;
  static constexpr unsigned int MAX_MESSAGE_LENGTH = 1024;
  static constexpr unsigned int MAX_NAME_LENGTH = 16;
};

#endif