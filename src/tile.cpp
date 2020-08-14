#include "tile.h"
#include "olcPixelGameEngine.h"
#include "sprites.h"

Tile::Tile(TileType type, int z, int x, int y, olc::Pixel tint) {
    this->tint = tint;
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

void Tile::draw(olc::PixelGameEngine * e, CamParams trx) {
    /*
       r += (r * (t->selected || t->hovered)) / 5;
	g += (g * (t->selected || t->hovered)) / 5;
	b += (b * (t->selected || t->hovered)) / 5;
	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;*/
	olc::vf2d v = this->getTextureCoordinates(trx);
	tileSprites[(int)type].draw(e, trx, v, this);
}

olc::vf2d Tile::getTextureCoordinates() {
	int sx = x * TEXTURE_W / 2;
	int sy = y * TEXTURE_H;

	float scx = sx - sy;
	float scy = (sx + sy - TEXTURE_Z * z) / 2;
	return olc::vf2d(scx, scy);
}