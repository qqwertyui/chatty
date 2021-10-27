### Chatty-server

Chat server implementation which serves mainly for educational purposes. It is multithreaded server which broadcasts messages sent by one user to others. It also supports few simple commands (e.g. /online to check who is online).

## 1. Dependencies:
a) Client:
- spdlog
- pdcurses
- winsock

b) Server:
- spdlog
- winsock2
- fmt

## 2. Build:
git clone https://github.com/qqwertyui/chatty-server.git && cd chatty-server
make

## 3. Usage:
### Force chatty to listen on localhost
- ./chatty_server.exe 127.0.0.1

### Force chatty to listen on public interface
- ./chatty_server.exe <your_public_ip>

### Connect to running server
- ./chatty_client.exe <server_ip> <username>

##### To leave client use /exit command