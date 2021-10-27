CC = g++
CFLAGS = -Wall -Wextra -std=c++17 -Wno-pmf-conversions
LIB = -lspdlog -lfmt -pthread
SRC = $(shell ls src/*.cpp) $(shell ls ../shared/src/*.cpp)
INCLUDE = -I./include -I../shared/include

ifeq ($(OS),Windows_NT)
	LIB += -lws2_32 -lpdcurses
	EXTENSION = .exe
else
	LIB += -lncurses
endif

FILENAME = chatty_client$(EXTENSION)

all:
	$(CC) $(CFLAGS) $(SRC) $(INCLUDE) $(LIB) -o $(FILENAME)

clean:
	rm $(FILENAME) -rf