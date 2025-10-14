.PHONY:all
all : publish consume 

publish : publish.cpp
	g++ -o $@ $^ -std=c++17 -lamqpcpp -lev -lpthread  
consume : consume.cpp
	g++ -o $@ $^ -std=c++17 -lamqpcpp -lev -lpthread 