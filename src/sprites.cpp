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

void TileSprite::draw(olc::PixelGameEngine * e, CamParams trx, olc::vf2d pos, olc::Pixel tint) {

    if (drawGround != TileType::VOID) {
		 // e->DrawDecal(pos, tileSprites[(int)drawGround], {trx.zoom, trx.zoom}, t->tint);
         tileSprites[(int)drawGround].draw(e, trx, pos, tint);
    }

    for (TileSpriteComponent &c : components) {
        float scl = trx.zoom / (c.width / 128.0f);
        if (c.tint) {
            e->DrawDecal(pos, c.decal, {scl, scl}, tint);
        } else {
            e->DrawDecal(pos, c.decal, {scl, scl});
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

    if (!parsingSuccessful) {
        std::cerr << "Unable to open texture json definition '" << fName << "'. Ignoring.\n";
    }

    afile.close();
    drawGround = (TileType)root["drawGround"].asInt();
    for (Json::Value t : root["textures"]) {
        olc::Decal * dec = new olc::Decal(new olc::Sprite(texturedir + "/" + t["imageFile"].asString()));
        bool tint = t["tint"].asBool();
        uint32_t width = t.get("width", 128).asInt();
        components.push_back({dec, tint, width});
    }
}
void registerTileSprite(std::string x) {
    tileSprites.push_back(TileSprite(x));
}

void registerUISprite(std::string filename, std::string name) {
	olc::Sprite * temp = new olc::Sprite(texturedir + "/" + filename);
	UIComponents[name] = {new olc::Decal(temp), olc::vi2d(3, 3)};
}

void loadSprites() {
	tileSprites.clear();
	std::string names[] = {"void.json", "ground.json", "bush.json", "tree.json", "pine.json", "water.json", "rock.json", "house.json", "pineforest.json"};
	for (int i = 0; i < *(&names + 1) - names; i++) {
		registerTileSprite(names[i]);
	}

	registerUISprite("menu_closed.png", "menu_closed");
	registerUISprite("menu_open.png", "menu_open");
	registerUISprite("menu_item.png", "menu_item");
}
