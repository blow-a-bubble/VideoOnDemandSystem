all: test_client test_server

test_client: test_client.cc echo.pb.cc ../../source/rpc.cc ../../source/logger.cc
	g++ -g -o $@ $^ -lbrpc -lleveldb -lgflags -lssl -lcrypto -lprotobuf -lspdlog -lfmt -std=c++17

test_server: test_server.cc echo.pb.cc ../../source/rpc.cc ../../source/logger.cc
	g++ -g -o $@ $^ -lbrpc -lleveldb -lgflags -lssl -lcrypto -lprotobuf -lspdlog -lfmt -std=c++17
%.pb.cc: %.proto
	protoc -I. --cpp_out=. $<
.PHONY: clean
clean:
	rm -f test_client test_server echo.pb.cc echo.pb.h
