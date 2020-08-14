#include "sprites.h"

#include "olcPixelGameEngine.h"

#include <string>
#include <vector>
#include <fstream>
#include <jsoncpp/json/json.h>

std::vector<olc::Sprite *> sprites;
std::vector<TileSprite> tileSprites;
std::unordered_map<std::string, UIComponent> UIComponents;
std::string texturedir = "textures";

void TileSprite::draw(olc::PixelGameEngine * e, CamParams trx, olc::vf2d pos, Tile * t) {

    if (drawGround != TileType::VOID) {
		 e->DrawDecal(v, tileSprites[(int)drawGround], {trx.zoom, trx.zoom}, getTint(t));
    }
	
    for (TileSpriteComponent &c : components) {
        if (c.tint) {
            e->DrawDecal(pos, c.decal, {trx.zoom, trx.zoom}, t->tint);
        } else {
            e->DrawDecal(pos, c.decal, {trx.zoom, trx.zoom});
        }
    }
}

TileSprite::TileSprite(std::string fName) {
    std::ifstream afile;
    afile.open(texturedir + "/" + fName);
    
    std::string content((std::istreambuf_iterator<char>(afile)), (std::istreambuf_iterator<char>()));

    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();

    Json::Value root;
    std::string errors;

    bool parsingSuccessful = reader->parse(
        content.c_str(),
        content.c_str() + content.size(),
        &root,
        &errors
    );
    delete reader;

    afile.close();
    
    for (Json::Value t : root["textures"]) {
        olc::Decal * dec = new olc::Decal(new olc::Sprite(t["imageFile"].asString()));
        bool tint = t["tint"].asBool();
        components.push_back({dec, tint});
    }
}
void registerTileSprite(std::string x) {
    olc::Sprite * spr = new olc::Sprite(texturedir + "/" + x);
    tileSprites.push_back()
    decals.push_back(new olc::Decal(sprites[sprites.size() - 1]));
}

void registerUISprite(std::string filename, std::string name) {
	olc::Sprite * temp = new olc::Sprite(texturedir + "/" + filename);
	UIComponents[name] = {new olc::Decal(temp), olc::vi2d(3, 3)};
}

void loadSprites() {
	tileSprites.clear();
	std::string names[] = {"ground_iso.png", "tree_iso.png", "water_iso.png", "rock_iso.png", "tree_trunk.png"};
	for (int i = 0; i < *(&names + 1) - names; i++) {
		registerTileSprite(names[i]);
	}

	registerUISprite("menu_closed.png", "menu_closed");
	registerUISprite("menu_open.png", "menu_open");
	registerUISprite("menu_item.png", "menu_item");
}
