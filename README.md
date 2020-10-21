# important

This is alpha level software, it probably doesn't work. There are many many bugs, creating issues is much appreciated however if it's really really obvious then I probably know about it already.

# planet-client

This is the client for my kinda weird and probably not gonna be finished planet-game. IDK why anyone is here but if they are then here are some build instructions.

## Build instructions

You must have these dependencies installed:  
    asio (may need to build it from source)  
    boost (Actually you might not need this, depending on if asio requires it)
    jsoncpp  
and perhaps others.  

### Windows

Building on windows is not currently supported. minwingw may work however IT HAS NOT BEEN TESTED!

### Linux (Mac?)

First you must compile the common library, then the client can be built.
```bash
git clone https://github.com/Jachdich/planet-client
git clone https://github.com/Jachdich/planet-common

cd planet-common
make

#warning: this installs to ../planet-client directory. Make sure you have that installed!
make install

cd ../planet-client

#optional: make -j# where # is the number of threads to use
make
make run
```

### "It's not running!"

You need the server started too. For that go to https://github.com/Jachdich/planet-server

### Linker error mentioning jsoncpp

You might have jsoncpp installed to a different location. It needs to be in `/usr/include/jsoncpp/json/` (or equivilent `include` directory). Some installs omit the `jsoncpp` which can break things, if that's the case for your install then refer to the instructions below.

A fix for Arch based distros and anyone else with the json issue:
mkdir -p /usr/include/jsoncpp
cp -r /usr/include/json /usr/include/jsoncpp

## olcPixelGameEngine

This project uses the [olcPixelGameEngine](https://github.com/OneLoneCoder/olcPixelGameEngine). A copy of the olcPixelGameEngine licence can be found at the top of the `olcPixelGameEngine.h` file.
