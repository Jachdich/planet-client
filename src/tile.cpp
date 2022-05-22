#include "tile.h"
#include "olcPixelGameEngine.h"
#include "sprites.h"
#include <stdio.h>
Tile::Tile(TileType type, int z, int x, int y, olc::Pixel tint) {
    this->tint = tint;
    this->type = type;
    this->z = z;
    this->x = x;
    this->y = y;
}

olc::vd2d Tile::getTextureCoordinates(const CamParams &trx) const {
	int sx = x * TEXTURE_W / 2;
	int sy = y * TEXTURE_H;

	double scx = (sx - sy) * trx.zoom + trx.tx;
	double scy = ((sx + sy - TEXTURE_Z * z) / 2) * trx.zoom + trx.ty;
	return olc::vd2d(scx, scy);
}

void Tile::draw(olc::PixelGameEngine * e, const CamParams &trx) const {
    int r,g,b;
    r = tint.r;
    g = tint.g;
    b = tint.b;
    r += (r * (selected || hovered)) / 5;
	g += (g * (selected || hovered)) / 5;
	b += (b * (selected || hovered)) / 5;
	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;
	olc::vd2d v = this->getTextureCoordinates(trx);
	tileSprites[(int)type].draw(e, trx, v, olc::Pixel(r, g, b), state);
	if (errMsg != "") {
        //e->DrawStringDecal(v, errMsg, olc::WHITE, {trx.zoom, trx.zoom});
        e->DrawDecal(v, icons["tile_error"], {trx.zoom * 2, trx.zoom * 2});
	}
}

olc::vi2d Tile::getTextureCoordinates() const {
	int sx = x * TEXTURE_W / 2;
	int sy = y * TEXTURE_H;

	int scx = sx - sy;
	int scy = (sx + sy - TEXTURE_Z * z) / 2;
	return olc::vi2d(scx, scy);
}

void Tile::addError(std::string msg) {
    errMsg = msg;
}
