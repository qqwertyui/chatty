#ifndef CHATBASE_HPP
#define CHATBASE_HPP

#include "SocketBase.hpp"
#include <cstdint>
#include <string>

class ChatBase {
public:
  struct HelloPacket {
    uint8_t version;
    uint8_t name_length;
  };

  struct StatusPacket {
    uint8_t code;
  };

  struct Status {
    static constexpr uint8_t OK = 0;
    static constexpr uint8_t USERNAME_IS_USED = 1;
    static constexpr uint8_t USERNAME_TOO_LONG = 2;
    static constexpr uint8_t FORBIDDEN_CHARACTERS = 3;
    static constexpr uint8_t INVALID_VERSION = 4;
  };
  
  static constexpr unsigned short DEFAULT_CHAT_PORT = 6000;
  static constexpr unsigned int STATUS_SIZE = sizeof(uint8_t);

  static constexpr uint8_t VERSION = 1;
  static constexpr unsigned int MAX_LINE_LENGTH = 1024;
  static constexpr unsigned int MAX_NAME_LENGTH = 16;
  static constexpr unsigned int TIMEOUT = (40 * 1000); // in miliseconds
};


#endif