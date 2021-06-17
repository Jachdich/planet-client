#ifndef __SPRITES_H
#define __SPRITES_H
#include <vector>
#include <string>
#include <unordered_map>
#include "olcPixelGameEngine.h"
#include "tile.h"

//maths helper?
struct AABB {
	olc::vd2d pos;
	olc::vd2d size;
	inline AABB(double x, double y, double w, double h) : pos{x, y}, size{w, h} {}
	inline AABB() {}

	inline bool isInside(olc::vd2d point) {
		if (point.x > pos.x &&
			point.x < pos.x + size.x &&
			point.y > pos.y &&
			point.y < pos.y + size.y) {
			return true;
		} else {
			return false;
		}
	}
};

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
    uint32_t animations;
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
extern std::unordered_map<std::string, olc::Decal*> icons;
#endif
