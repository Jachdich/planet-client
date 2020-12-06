SOURCES := $(shell find src -type f -name *.cpp)
HEADERS := $(shell find include -type f -name *.h)
OBJECTS := $(patsubst src/%,obj/%,$(SOURCES:.cpp=.o))

client: $(OBJECTS)
	g++ $(OBJECTS) -o $@ -lX11 -lGL -lpthread -lpng -lstdc++fs -ljsoncpp -lssl -lcrypto

obj/%.o: src/%.cpp $(HEADERS)
	@mkdir -p obj
	g++ -c -o $@ $< -Wall -Werror -g -ggdb -std=c++17 -Iinclude
#	g++ -c -o $@ $< -Wall -Werror -O3 -std=c++17 -Iinclude

obj/FastNoise.o: src/FastNoise.cpp
	g++ -c -o $@ $< -Wall -g -O3 -Iinclude

debug: client
	gdb client

run: client
	./client

clean:
	rm obj/*.o
	rm client

.PHONY: clean
