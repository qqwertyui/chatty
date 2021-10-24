### Chatty-server

Chat server implementation which serves mainly for educational purposes. It is multithreaded server which broadcasts messages sent by one user to others. It also supports few simple commands (e.g. /online to check who is online).
This repository contains server only related files and source codes. Example client written in QT will be avalible soon.

## 1. Dependencies:
- fmt
- spdlog
- winsock2

## 2. Build:
git clone https://github.com/qqwertyui/chatty-server.git && cd chatty-server
make

## 3. Usage:
### Force chatty to listen on localhost
./chatty_server.exe 127.0.0.1

### Force chatty to listen on public interface
./chatty_server.exe <your_public_ip>