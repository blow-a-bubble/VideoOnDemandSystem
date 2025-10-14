server: server.cc ../../source/ffmpeg.cc ../../source/util.cc ../../source/logger.cc
	g++ -o $@ $^ -std=c++17 -lavcodec -lavformat -lavutil -lspdlog -lfmt -ljsoncpp
clean:
	rm -f server
