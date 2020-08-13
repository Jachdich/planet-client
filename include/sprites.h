#ifndef __SPRITES_H
#define __SPRITES_H
#include <vector>
#include <string>
#include <unordered_map>
#include "olcPixelGameEngine.h"

struct UIComponent {
	olc::Decal * decal;
	olc::vi2d textPos;
};

void loadSprites();
extern std::vector<olc::Sprite *> sprites;
extern std::vector<olc::Decal *> decals;
extern std::unordered_map<std::string, UIComponent> UIComponents;

#endif
