CC = g++
CFLAGS = -Wall -Wextra -std=c++17 -I./include -Wno-pmf-conversions
LIB = -lws2_32 -lspdlog -lfmt
SERVER_FILES = $(shell ls src/*.cpp)
FILENAME = chatty_server.exe

all:
	$(CC) $(CFLAGS) $(SERVER_FILES) $(LIB) -o $(FILENAME)

clean:
	rm $(FILENAME) -rf