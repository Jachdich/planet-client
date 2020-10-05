SOURCES := $(shell find src -type f -name *.cpp)
HEADERS := $(shell find include -type f -name *.h)
OBJECTS := $(patsubst src/%,obj/%,$(SOURCES:.cpp=.o))

client: $(OBJECTS) common.a
	g++ $(OBJECTS) common.a -o $@ -lX11 -lGL -lpthread -lpng -lstdc++fs -ljsoncpp

obj:
	mkdir -p obj

obj/%.o: src/%.cpp $(HEADERS) obj
	g++ -c -o $@ $< -Wall -Werror -g -ggdb -std=c++17 -Iinclude

obj/FastNoise.o: src/FastNoise.cpp obj
	g++ -c -o $@ $< -Wall -g -O3 -Iinclude

debug: client
	gdb client

run: client
	./client

.PHONY: runtime_ld_error
runtime_ld_error:
	echo "Warning: Workaround for my specific machine. MAY BREAK THINGS!"
	export LD_LIBRARY_PATH=/usr/local/lib64
	./client

clean:
	rm obj/*.o
	rm client

.PHONY: clean
