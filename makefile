SOURCES := src/game.cpp src/helperfunctions.cpp src/menu.cpp src/network.cpp src/planet.cpp src/planetdata.cpp src/planethud.cpp src/planetsurface.cpp src/sector.cpp src/sectorcache.cpp src/sprites.cpp src/star.cpp src/tile.cpp
HEADERS := include/client.h include/game.h include/helperfunctions.h include/menu.h include/network.h include/olcPixelGameEngine.h include/planet.h include/planetdata.h include/planethud.h include/planetsurface.h include/sector.h include/sectorcache.h include/sprites.h include/star.h include/tile.h
OBJECTS := $(patsubst src/%,obj/%,$(SOURCES:.cpp=.o))
OBJECTS_OPTIMISED := $(patsubst src/%,obj/optimised/%,$(SOURCES:.cpp=.o)) obj/optimised/client.o

LIBS := -lX11 -lGL -lpthread -lpng -lstdc++fs -ljsoncpp -lssl -lcrypto

client: $(OBJECTS) obj/client.o
	g++ $(OBJECTS) obj/client.o -o $@ $(LIBS)

surface_renderer: $(OBJECTS) obj/surfacerenderer.o
	g++ $(OBJECTS) obj/surfacerenderer.o -o $@ $(LIBS)

obj/%.o: src/%.cpp $(HEADERS)
	@mkdir -p obj
	g++ -c -o $@ $< -Wall -Werror -Wno-unknown-pragmas -g -ggdb -std=c++17 -Iinclude

obj/optimised/%.o: src/%.cpp $(HEADERS)
	@mkdir -p obj/optimised
	g++ -c -o $@ $< -Wall -Werror -Wno-unknown-pragmas -O3 -std=c++17 -Iinclude

debug: client
	gdb --args client 127.0.0.1

.PHONY: release
release: $(OBJECTS_OPTIMISED)
	g++ $(OBJECTS_OPTIMISED) -o planet-client -lX11 -lGL -lpthread -lpng -lstdc++fs -ljsoncpp -lssl -lcrypto

.PHONY: run
run: client
	vblank_mode=0 ./client

.PHONY: connect
connect: client
	vblank_mode=0 ./client 127.0.0.1 KingJellyfish password
	
.PHONY: clean
clean:
	rm obj/*.o
	rm client

