#ifndef __TILE_H
#define __TILE_H
#include "olcPixelGameEngine.h"
#include "helperfunctions.h"
#include "common/enums.h"

#define TEXTURE_W 128
#define TEXTURE_H 64
#define TEXTURE_Z 30

class Tile {
public:
	TileType type;
	int z;
	bool hovered = false;
	bool selected = false;
	int x,y;
	olc::Pixel tint;
	Tile(TileType type, int z, int x, int y, olc::Pixel tint);
	olc::vf2d getTextureCoordinates(CamParams trx);
	olc::vf2d getTextureCoordinates();
	void draw(olc::PixelGameEngine * e, CamParams trx);
};

#endif
