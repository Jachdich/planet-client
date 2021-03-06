#include "sprites.h"

#include "olcPixelGameEngine.h"

#include <string>
#include <vector>
#include <fstream>
#include <jsoncpp/json/json.h>

std::vector<olc::Sprite *> sprites;
std::vector<TileSprite> tileSprites;
std::unordered_map<std::string, UIComponent> UIComponents;
std::unordered_map<std::string, MenuComponent> menuComponents;
std::string texturedir = "textures";

Json::Value getJsonFromTextureFile(std::string fName) {
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
        //TODO actually ignore the error
    }

    afile.close();
    return root;
}

void TileSprite::draw(olc::PixelGameEngine * e, CamParams trx, olc::vf2d pos, olc::Pixel tint) {

    if (drawGround != TileType::VOID) {
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
	Json::Value root = getJsonFromTextureFile(fName);
    drawGround = (TileType)root["drawGround"].asInt();
    for (Json::Value t : root["textures"]) {
        olc::Sprite * spr = new olc::Sprite(texturedir + "/" + t["imageFile"].asString());
        olc::Decal * dec = new olc::Decal(spr);
        bool tint = t["tint"].asBool();
        uint32_t width = t.get("width", 128).asInt();
        components.push_back({dec, tint, width});
        sprites.push_back(spr);
    }
}
void registerTileSprite(std::string x) {
    tileSprites.push_back(TileSprite(x));
}

void registerUISprite(std::string filename, std::string name) {
	olc::Sprite * temp = new olc::Sprite(texturedir + "/" + filename);
	UIComponents[name] = {new olc::Decal(temp), olc::vi2d(3, 3)};
	sprites.push_back(temp);
}

void registerMenuSprite(std::string filename, std::string name) {
	Json::Value root = getJsonFromTextureFile(filename);
	olc::Sprite * temp = new olc::Sprite(texturedir + "/" + root["texture"].asString());
	sprites.push_back(temp);
	MenuComponent c;
	c.decal = new olc::Decal(temp);
	for (Json::Value v : root["buttons"]) {
		c.buttons[v["name"].asString()] = AABB(v["pos_x"].asDouble(),
								  			   v["pos_y"].asDouble(),
											   v["width"].asDouble(),
								  			   v["height"].asDouble());
	}
	menuComponents[name] = c;
}

void loadSprites() {
	for (TileSprite t : tileSprites) {
		for (TileSpriteComponent tc : t.components) {
			delete tc.decal;
		}
	}

	for (std::pair<std::string, UIComponent> element : UIComponents) {
		delete element.second.decal;
	}
	for (std::pair<std::string, MenuComponent> element : menuComponents) {
		delete element.second.decal;
	}
	
	tileSprites.clear();
	UIComponents.clear();
	menuComponents.clear();
	for (olc::Sprite * spr : sprites) {
	    delete spr;
	}
	sprites.clear();
	std::string names[] = {"void.json", "ground.json", "bush.json", "tree.json", "pine.json",
	                       "water.json", "rock.json", "house.json", "pineforest.json", "forest.json", "tonk.json"};
	for (int i = 0; i < *(&names + 1) - names; i++) {
		registerTileSprite("tiles/json/" + names[i]);
	}

	registerUISprite("hud/menu_closed.png", "menu_closed");
	registerUISprite("hud/menu_open.png", "menu_open");
	registerUISprite("hud/menu_item.png", "menu_item");
	registerUISprite("hud/error_popup.png", "error_popup");

	registerMenuSprite("menu/main/main.json", "main");
	registerMenuSprite("menu/multiplayer/multiplayer.json", "multiplayer");
	registerMenuSprite("menu/background.json", "background");
	registerMenuSprite("menu/multiplayer/serverconnect.json", "serverconnect");
}
