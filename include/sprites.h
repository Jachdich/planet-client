#ifndef __SPRITES_H
#define __SPRITES_H
#include <vector>
#include <string>
#include <unordered_map>
#include "olcPixelGameEngine.h"
#include "tile.h"
#include "MathUtils.h"

//maths helper?

struct MenuComponent {
	olc::Decal * decal;
	std::unordered_map<std::string, AABB> buttons;
};

struct UIComponent {
	olc::Decal * decal;
	olc::vi2d textPos;
	olc::vi2d size;
};

struct TileSpriteComponent {
    olc::Decal * decal;
    bool tint;
    uint32_t width;
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
extern std::unordered_map<std::string, MenuComponent> menuComponents;
#endif
