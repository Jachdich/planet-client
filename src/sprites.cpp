#include "sprites.h"

#include "olcPixelGameEngine.h"

std::vector<olc::Sprite *> sprites;
std::vector<olc::Decal *> decals;
std::unordered_map<std::string, UIComponent> UIComponents;
std::string texturedir = "textures";

void registerTileSprite(std::string x) {
	sprites.push_back(new olc::Sprite(texturedir + "/" + x));
    decals.push_back(new olc::Decal(sprites[sprites.size() - 1]));
}

void registerUISprite(std::string filename, std::string name) {
	olc::Sprite * temp = new olc::Sprite(texturedir + "/" + filename);
	UIComponents[name] = {new olc::Decal(temp), olc::vi2d(3, 3)};
}

void loadSprites() {
	sprites.clear();
	decals.clear();
	std::string names[] = {"ground_iso.png", "tree_iso.png", "water_iso.png", "rock_iso.png"};
	for (int i = 0; i < *(&names + 1) - names; i++) {
		registerTileSprite(names[i]);
	}

	registerUISprite("menu_closed.png", "menu_closed");
	registerUISprite("menu_open.png", "menu_open");
	registerUISprite("menu_item.png", "menu_item");
}
