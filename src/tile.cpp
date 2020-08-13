#include "tile.h"
#include "olcPixelGameEngine.h"

Tile::Tile(TileType type, int z, int x, int y) {
	this->type = type;
	this->z = z;
	this->x = x;
	this->y = y;
}

olc::vf2d Tile::getTextureCoordinates(CamParams trx) {
	int sx = x * TEXTURE_W / 2;
	int sy = y * TEXTURE_H;

	float scx = (sx - sy) * trx.zoom + trx.tx;
	float scy = ((sx + sy - TEXTURE_Z * z) / 2) * trx.zoom + trx.ty;
	return olc::vf2d(scx, scy);
}

olc::vf2d Tile::getTextureCoordinates() {
	int sx = x * TEXTURE_W / 2;
	int sy = y * TEXTURE_H;

	float scx = sx - sy;
	float scy = (sx + sy - TEXTURE_Z * z) / 2;
	return olc::vf2d(scx, scy);
}