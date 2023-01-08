#include "../include/sprites.h"
#include "../include/common/resources.h"

#include "../include/olcPixelGameEngine.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <jsoncpp/json/json.h>

std::vector<olc::Sprite *> sprites;
std::vector<TileSprite> tileSprites;
std::unordered_map<std::string, UIComponent> UIComponents;
std::unordered_map<std::string, MenuComponent> menuComponents;
std::unordered_map<std::string, olc::Decal*> icons;
std::string texturedir = "textures";

std::unordered_map<int64_t, olc::Decal*> cached_tinted_decals;
std::vector<olc::Sprite*> cached_tinted_sprites;


typedef struct {
    double r;       // a fraction between 0 and 1
    double g;       // a fraction between 0 and 1
    double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;

static hsv   rgb2hsv(rgb in);
static rgb   hsv2rgb(hsv in);

hsv rgb2hsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}


rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;     
}
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
            uint64_t t_id = (uint64_t)c.id << 32 | tint.n;
            if (cached_tinted_decals.count(t_id) != 0) {
                e->DrawDecal(pos, cached_tinted_decals[t_id], {scl, scl});
            } else {
                olc::Sprite *s = new olc::Sprite(c.width, c.decal->sprite->height);
                e->SetDrawTarget(s);
                e->SetPixelMode(olc::Pixel::NORMAL);
                e->DrawPartialSprite({0, 0}, c.decal->sprite,
                    olc::vi2d((trx.animationStage / c.animationSpeed) % c.animations, 0) * c.width, {(int)c.width, (int)c.decal->sprite->height});

                hsv t_hsv = rgb2hsv(rgb { tint.r / 255.0, tint.g / 255.0, tint.b / 255.0 });

                for (uint32_t x = 0; x < c.width; x++) {
                    for (int32_t y = 0; y < c.decal->sprite->height; y++) {
                        olc::Pixel p = s->GetPixel(x, y);
                        float dh = ((int)p.r - 128) / 127.0 * 360;
                        float ds = ((int)p.g - 128) / 127.0;
                        float dv = ((int)p.b - 128) / 127.0;
                        float h = t_hsv.h + dh;
                        float s_ = t_hsv.s + ds;
                        float v = t_hsv.v + dv;
                        if (h < 0) h = 0;
                        if (h > 359) h = 359;
                        if (s_ < 0) s_ = 0;
                        if (s_ > 1) s_ = 1;
                        if (v < 0) v = 0;
                        if (v > 1) v = 1;
                        hsv p_hsv = hsv { h, s_, v };
                        rgb p_rgb = hsv2rgb(p_hsv);
                        s->SetPixel(x, y, olc::Pixel(p_rgb.r * 255, p_rgb.g * 255, p_rgb.b * 255, p.a));
                    }
                }
                e->SetDrawTarget(nullptr);
                olc::Decal *d = new olc::Decal(s);
                cached_tinted_decals[t_id] = d;
                cached_tinted_sprites.push_back(s);
                e->DrawDecal(pos, d, {scl, scl});
            }
            //e->DrawDecal(pos, c.decal, {scl, scl}, tint);
        } else {
            e->DrawPartialDecal(pos, c.decal,
                olc::vi2d((trx.animationStage / c.animationSpeed) % c.animations, 0) * c.width, {(float)c.width, (float)c.decal->sprite->height},
                {scl, scl});
            //e->DrawDecal(pos, c.decal, {scl, scl});
        }
    }
}

uint32_t sprite_component_id = 0;

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
            olc::Sprite * spr = load_sprite(texturedir + "/" + t["imageFile"].asString());
            olc::Decal * dec = new olc::Decal(spr);
            bool tint = t["tint"].asDouble();
            uint32_t animationSpeed = t.get("animationSpeed", 1).asUInt();
            uint32_t width = t.get("width", 128).asInt();
            uint32_t realWidth = spr->width;
            uint32_t animations = realWidth / width;
            if (animations == 0) animations = 1;
            state.components.push_back({dec, spr, tint, animationSpeed, width, animations, sprite_component_id++});
            sprites.push_back(spr);
        }
        states.push_back(state);
    }
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
        tileSprites.push_back(TileSprite("tiles/json/" + names[i]));
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
