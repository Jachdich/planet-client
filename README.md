# important

This is alpha level software, it probably doesn't work. I may commit broken code because I cba making a new branch. There are many many bugs, creating issues is much appreciated however if it's really really obvious then I probably know about it already.

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

You might have jsoncpp installed to a different location. It needs to be in `/usr/include/jsoncpp/json/` (or equivilent `include` directory). Some installs omit the `jsoncpp` which can break things, if that's the case for your install then refer to the instructions below.

A fix for Arch based distros and anyone else with the json issue:
mkdir -p /usr/include/jsoncpp
cp -r /usr/include/json /usr/include/jsoncpp
