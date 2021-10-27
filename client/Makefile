CC = g++
CFLAGS = -Wall -Wextra -std=c++17 -I./include -I../shared/include -Wno-pmf-conversions
LIB = -lws2_32 -lspdlog -lpdcurses
CLIENT_FILES = $(shell ls src/*.cpp) $(shell ls ../shared/src/*.cpp)
FILENAME = chatty_client.exe

all:
	$(CC) $(CFLAGS) $(CLIENT_FILES) $(LIB) -o $(FILENAME)

clean:
	rm $(FILENAME) -rf