FROM ubuntu:20.04
COPY server /app/server/
COPY shared /app/shared/
WORKDIR /app/server
RUN apt-get update && apt-get install make g++ libfmt-dev libspdlog-dev -y
RUN make
CMD /app/server/chatty_server 0.0.0.0