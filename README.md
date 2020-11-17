# Important

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


## Textures

To make your own textures it's fairly simple: each tile has a JSON file, and the JSON file describes one or more image files that are drawn at the position of a tile.

The textures you need to create are:
 - bush.json
 - ground.json
 - house.json
 - pine.json
 - pineforest.json
 - rock.json
 - tree.json
 - forest.json
 - void.json (no texture)
 - water.json

Here's an example, I'll break down what each bit does.

```json
{
    "drawGround": true,
    "textures": [
        {
            "imageFile": "tree_iso.png",
            "tint": true,
            "width": 128
        },
        {
            "imageFile": "tree_trunk.png",
            "tint": false,
            "width": 128
        }
    ]
}
```

There are two main values to set in the json file: `drawGround` and `textures`.

### drawGround

drawGround is fairly self explanatory, `true` if you want to draw the ground sprite under your sprite (useful for things such as trees) or `false` if not.

### textures

This is where all the customisation happens: there are three values, `imageFile`, `tint` and `width`. The textures will be drawn in the order that you put them in the list.

#### imageFile

The path to the image file. It can contain `/` if you want your image files in subdirectories.

#### tint

Whether or not to tint the image to the colour at the tile's position on the planet surface.

#### width

The width of the tile, in pixels. This is half of the height of a square tile.

### considerations when creating a texture

The texture is drawn one tile below the top of the image, to allow for tall tiles like trees.
More to come.
