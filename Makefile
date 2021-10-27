all: client server

client:
	make -C client

server:
	make -C server

.PHONY: all client server