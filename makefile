SOURCES := $(shell find src -type f -name *.cpp)
HEADERS := $(shell find include -type f -name *.h)
OBJECTS := $(patsubst src/%,obj/%,$(SOURCES:.cpp=.o))

client: $(OBJECTS)
	g++ $(OBJECTS) -o $@ -lX11 -lGL -lpthread -lpng -lstdc++fs -ljsoncpp

obj/%.o: src/%.cpp $(HEADERS)
	g++ -c -o $@ $< -Wall -Werror -g -ggdb -std=c++17 -Iinclude

obj/FastNoise.o: src/FastNoise.cpp
	g++ -c -o $@ $< -Wall -g -O3 -Iinclude

debug: client
	gdb client

run: client
	./client

.PHONY: runtime_ld_error
runtime_ld_error:
	echo "Warning: Workaround for my specific machine. MAY BREAK THINGS!"
	export LD_LIBRARY_PATH=/usr/local/lib64

clean:
	rm obj/*.o
	rm client

.PHONY: clean
