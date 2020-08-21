#ifndef __SPRITES_H
#define __SPRITES_H
#include <vector>
#include <string>
#include <unordered_map>
#include "olcPixelGameEngine.h"
#include "tile.h"

struct UIComponent {
	olc::Decal * decal;
	olc::vi2d textPos;
};

struct TileSpriteComponent {
    olc::Decal * decal;
    bool tint;
};

struct TileSprite {
	TileType drawGround;
	std::vector<TileSpriteComponent> components;
	void draw(olc::PixelGameEngine * e, CamParams trx, olc::vf2d pos, olc::Pixel tint);
	TileSprite(std::string fName);
};

void loadSprites();
extern std::vector<olc::Sprite *> sprites;
extern std::vector<TileSprite> tileSprites;
extern std::unordered_map<std::string, UIComponent> UIComponents;

#endif
