all: client server

client: client.cc
	g++ -o $@ $^ -std=c++17 -pthread 

server: server.cc
	g++ -o $@ $^ -std=c++17 -pthread 
.PHONY: clean
clean:
	rm -f client server	
