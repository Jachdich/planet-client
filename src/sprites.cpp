#include "sprites.h"
#include "common/resources.h"

#include "olcPixelGameEngine.h"

#include <string>
#include <vector>
#include <fstream>
#include <jsoncpp/json/json.h>

std::vector<olc::Sprite *> sprites;
std::vector<TileSprite> tileSprites;
std::unordered_map<std::string, UIComponent> UIComponents;
std::unordered_map<std::string, MenuComponent> menuComponents;
std::unordered_map<std::string, olc::Decal*> icons;
std::string texturedir = "textures";

olc::Sprite *load_sprite(const std::string &fname) {
    olc::Sprite *spr = new olc::Sprite();
    olc::rcode status = spr->LoadFromFile(fname);
    if (status == olc::rcode::NO_FILE) {
        std::cerr << "Unable to load texture '" << fname << "': File not found\n";
    } else if (status == olc::rcode::FAIL) {
        std::cerr << "Unable to load texture '" << fname << "': Other error (invalid/corrupted file?)\n";
    }
    return spr;
}

Json::Value getJsonFromTextureFile(const std::string &fName) {
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

void TileSprite::draw(olc::PixelGameEngine * e, const CamParams &trx, const olc::vd2d &pos, const olc::Pixel &tint, uint16_t state_idx) {
    TileSpriteState &state = states[state_idx];

    if (state.drawGround != TILE_AIR) {
         tileSprites[(int)state.drawGround].draw(e, trx, pos, tint, 0);
    }

    for (TileSpriteComponent &c : state.components) {
        float scl = trx.zoom / (c.width / 128.0f);
        if (c.tint) {
            e->DrawPartialDecal(pos, c.decal,
                olc::vi2d((trx.animationStage / c.animationSpeed) % c.animations, 0) * c.width, {(float)c.width, (float)c.decal->sprite->height},
                {scl, scl}, tint);
            //e->DrawDecal(pos, c.decal, {scl, scl}, tint);
        } else {
            e->DrawPartialDecal(pos, c.decal,
                olc::vi2d((trx.animationStage / c.animationSpeed) % c.animations, 0) * c.width, {(float)c.width, (float)c.decal->sprite->height},
                {scl, scl});
            //e->DrawDecal(pos, c.decal, {scl, scl});
        }
    }
}

TileSprite::TileSprite(std::string fName) {
	Json::Value definition = getJsonFromTextureFile(fName);

    Json::Value e;
	if (definition["states"].isNull()) {
	    e["states"][0] = definition;
	} else {
        e = definition;
	}

    for (Json::Value root : e["states"]) {
        TileSpriteState state;
        //TODO this is a very bad idea!
        //TODO figure out why it's a bad idea?
        //TODO the analysis is severely limited by my lack of understandinmg of what I am doing
        state.drawGround = (TileType)root["drawGround"].asInt();

        for (Json::Value t : root["textures"]) {
            olc::Sprite *spr = load_sprite(texturedir + "/" + t["imageFile"].asString());
            olc::Decal *dec = new olc::Decal(spr);
            bool tint = t["tint"].asDouble();
            uint32_t animationSpeed = t.get("animationSpeed", 1).asUInt();
            uint32_t width = t.get("width", 128).asInt();
            uint32_t realWidth = spr->width;
            uint32_t animations = realWidth / width;
            if (animations == 0) animations = 1;
            state.components.push_back({dec, spr, tint, animationSpeed, width, animations});
            sprites.push_back(spr);
        }
        states.push_back(state);
    }
}
void registerTileSprite(const std::string &x) {
    tileSprites.push_back(TileSprite(x));
}

void registerUISprite(const std::string &filename, const std::string &name, olc::vi2d text_offset) {
	olc::Sprite * temp = load_sprite(texturedir + "/" + filename);
	UIComponents[name] = {new olc::Decal(temp), text_offset};
	sprites.push_back(temp);
}

void registerMenuSprite(const std::string &filename, const std::string &name) {
	Json::Value root = getJsonFromTextureFile(filename);
	olc::Sprite * temp = load_sprite(texturedir + "/" + root["texture"].asString());
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

void registerIconSprite(const std::string &name) {
    olc::Sprite *spr = load_sprite(texturedir + "/hud/icons/" + name + ".png");
    olc::Decal *dec = new olc::Decal(spr);
    icons[name] = dec;
    sprites.push_back(spr);
}

void loadSprites() {
	for (TileSprite t : tileSprites) {
	    for (TileSpriteState state: t.states) {
    		for (TileSpriteComponent tc : state.components) {
    			delete tc.decal;
    		}
		}
	}

	for (std::pair<std::string, UIComponent> element : UIComponents) {
		delete element.second.decal;
	}
	for (std::pair<std::string, MenuComponent> element : menuComponents) {
		delete element.second.decal;
	}
	for (auto &[k, v] : icons) {
	    delete v;
	}
	
	tileSprites.clear();
	UIComponents.clear();
	menuComponents.clear();
	icons.clear();
	for (olc::Sprite * spr : sprites) {
	    delete spr;
	}

	sprites.clear();
	std::string names[] = {"void.json", "ground.json", "bush.json", "tree.json", "pine.json",
	                       "water.json", "rock.json", "house.json", "pineforest.json", "forest.json", "tonk.json",
	                       "farm.json", "greenhouse.json", "waterpump.json", "mine.json", "blastfurnace.json", "warehouse.json", "forestry.json",
	                       "capsule.json", "road.json", "pipe.json", "cable.json", "powerstation.json"};
	//sidenote: what the fuck?
	//the hell does this mean?
	for (int i = 0; i < *(&names + 1) - names; i++) {
		registerTileSprite("tiles/json/" + names[i]);
	}

	registerUISprite("hud/menu_closed.png", "menu_closed", olc::vi2d(3, 3));
	registerUISprite("hud/menu_open.png", "menu_open", olc::vi2d(3, 3));
	registerUISprite("hud/menu_item.png", "menu_item", olc::vi2d(6, 3));
	registerUISprite("hud/error_popup.png", "error_popup", olc::vi2d(3, 3));
	registerUISprite("menu/cursor.png", "cursor", olc::vi2d(0, 0));

	registerMenuSprite("menu/main/main.json", "main");
	registerMenuSprite("menu/multiplayer/multiplayer.json", "multiplayer");
	registerMenuSprite("menu/background.json", "background");
	registerMenuSprite("menu/multiplayer/serverconnect.json", "serverconnect");
	registerMenuSprite("menu/login/login.json", "login");

    for (int i = 0; i < NUM_RESOURCES; i++) {
        if (i == RES_PEOPLE_IDLE) continue;
        registerIconSprite(std::string(res_names[i]));
    }

    registerIconSprite("tile_error");
}
