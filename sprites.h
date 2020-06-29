#ifndef __SPRITES_H
#define __SPRITES_H
#include <vector>
#include "olcPixelGameEngine.h"

void loadSprites();
extern std::vector<olc::Sprite *> sprites;
extern std::vector<olc::Decal *> decals;

#endif
