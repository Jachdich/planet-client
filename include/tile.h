#ifndef __TILE_H
#define __TILE_H
#include "olcPixelGameEngine.h"
#include "helperfunctions.h"
#include "common/enums.h"

#define TEXTURE_W 128
#define TEXTURE_H 64
#define TEXTURE_Z 20

class Tile {
public:
	TileType type;
	int z;
	bool hovered = false;
	bool selected = false;
	int x,y;
	uint16_t state = 0;
	std::string errMsg = "";
	olc::Pixel tint;
	Tile(TileType type, int z, int x, int y, olc::Pixel tint);
	olc::vd2d getTextureCoordinates(const CamParams &trx) const;
	olc::vi2d getTextureCoordinates() const;
	void draw(olc::PixelGameEngine * e, const CamParams &trx) const;
	void addError(std::string msg);
};

#endif
