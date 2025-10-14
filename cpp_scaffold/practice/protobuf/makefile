all: proto_json
proto_json: example.pb.cc proto_json.cpp
	g++ -o $@ $^ -lprotobuf -ljsoncpp
clean:
	rm -f proto_json
	rm -f example.pb.cc example.pb.h
%.pb.cc %.pb.h: %.proto
	protoc --cpp_out=. $@