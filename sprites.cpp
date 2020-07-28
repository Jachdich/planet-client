#include "sprites.h"

#include "olcPixelGameEngine.h"

std::vector<olc::Sprite *> sprites;
std::vector<olc::Decal *> decals;
std::unordered_map<std::string, UIComponent> UIComponents;

void registerTileSprite(std::string x) {
	sprites.push_back(new olc::Sprite(x));
    decals.push_back(new olc::Decal(sprites[sprites.size() - 1]));
}

void registerUISprite(std::string filename, std::string name) {
	olc::Sprite * temp = new olc::Sprite(filename);
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
	/*
    sprites.push_back(new olc::Sprite("ground_iso.png"));
    decals.push_back(new olc::Decal(sprites[sprites.size() - 1]));
    
    sprites.push_back(new olc::Sprite("tree_iso.png"));
    decals.push_back(new olc::Decal(sprites[sprites.size() - 1]));
	
	sprites.push_back(new olc::Sprite("water_iso.png"));
    decals.push_back(new olc::Decal(sprites[sprites.size() - 1]));
	
	sprites.push_back(new olc::Sprite("rock_iso.png"));
    decals.push_back(new olc::Decal(sprites[sprites.size() - 1]));
	
	sprites.push_back(new olc::Sprite("pixel.png"));
    decals.push_back(new olc::Decal(sprites[sprites.size() - 1]));
	*/
}
