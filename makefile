SOURCES = $(wildcard *.cpp)
HEADERS = $(SOURCES:.cpp=.h)
OBJECTS = $(SOURCES:.cpp=.o)

OBJ_DIR = obj


client: $(OBJECTS)
	g++ $(OBJECTS) -o $@ -lX11 -lGL -lpthread -lpng -lstdc++fs -ljsoncpp

%.o: %.cpp $(HEADERS)
	g++ -c -o $@ $< -Wall -g -std=c++17

FastNoise.o: FastNoise.cpp
	g++ -c -o $@ $< -Wall -g -O3

debug: client
	gdb client

run: client
	./client

clean:
	rm *.o
	rm client
