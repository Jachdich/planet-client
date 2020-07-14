# planet-client

This is the client for my kinda weird and probably not gonna be finished planet-game. IDK why anyone is here but if they are then here are some build instructions.

## Build instructions

You must have these dependencies installed:  
    asio (may need to build it from source)  
    boost  
    jsoncpp  
and perhaps others.  

### Windows

Building on windows is not currently supported. minwingw may work however IT HAS NOT BEEN TESTED!

### Linux (Mac?)

Just run `make`. Simple as that. This generates a binary called `client` which can be run.

### "It's not running!"

You need the server started too. For that go to https://github.com/Jachdich/planet-server

### Linker error mentioning jsoncpp

Ugh IDK how to fix this but you might have jsoncpp installed to a different location. It needs to be in `/usr/include/jsoncpp/json/` (or equivilent `include` directory). Some installs omit the final `json` which can break things, if that's the case for your install just `mkdir /tmp/json && mv /usr/include/jsoncpp/* /tmp/json && mv /tmp/json /usr/include/jsoncpp` or something else if your install is weird.

## olcPixelGameEngine

This project uses the [olcPixelGameEngine](ggegeag). A copy of the olcPixelGameEngine licence can be found in `LICENCE-OLCPIXELGAMEENGINE.md`.


