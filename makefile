SOURCES := src/FastNoise.cpp src/client.cpp src/game.cpp src/helperfunctions.cpp src/menu.cpp src/network.cpp src/planet.cpp src/planetdata.cpp src/planethud.cpp src/planetsurface.cpp src/sector.cpp src/sectorcache.cpp src/sprites.cpp src/star.cpp src/tile.cpp
HEADERS := include/FastNoise.h include/client.h include/game.h include/helperfunctions.h include/menu.h include/network.h include/olcPixelGameEngine.h include/planet.h include/planetdata.h include/planethud.h include/planetsurface.h include/sector.h include/sectorcache.h include/sprites.h include/star.h include/tile.h
OBJECTS := $(patsubst src/%,obj/%,$(SOURCES:.cpp=.o))
OBJECTS_OPTIMISED := $(patsubst src/%,obj/optimised/%,$(SOURCES:.cpp=.o))

client: $(OBJECTS)
	g++ $(OBJECTS) -o $@ -lX11 -lGL -lpthread -lpng -lstdc++fs -ljsoncpp -lssl -lcrypto

obj/%.o: src/%.cpp $(HEADERS)
	@mkdir -p obj
	g++ -c -o $@ $< -Wall -Werror -g -ggdb -std=c++17 -Iinclude

obj/optimised/%.o: src/%.cpp $(HEADERS)
	@mkdir -p obj/optimised
	g++ -c -o $@ $< -Wall -Werror -O3 -std=c++17 -Iinclude

obj/FastNoise.o: src/FastNoise.cpp
	g++ -c -o $@ $< -Wall -g -O3 -Iinclude

debug: client
	gdb --args client 127.0.0.1

.PHONY: release
release: $(OBJECTS_OPTIMISED)
	g++ $(OBJECTS_OPTIMISED) -o planet-client -lX11 -lGL -lpthread -lpng -lstdc++fs -ljsoncpp -lssl -lcrypto

.PHONY: run
run: client
	./client

.PHONY: connect
connect: client
	./client 127.0.0.1
	
.PHONY: clean
clean:
	rm obj/*.o
	rm client

