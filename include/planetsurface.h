#ifndef __PLANETSURFACE_H
#define __PLANETSURFACE_H

#include <jsoncpp/json/json.h>

#include "helperfunctions.h"
#include "olcPixelGameEngine.h"
#include "planethud.h"
//#include "planet.h"

class Planet;

struct Stats {
    uint32_t wood;
    uint32_t stone;
};

class PlanetSurface {
public:
    std::vector<Tile> tiles;
    Stats stats;
	int rad;
	int pos;
	int lastSelectX = 0;
	int lastSelectY = 0;
    bool generated = false;
    bool requested = false;
	Planet * parent;
	PlanetHUD * hud;
	PlanetData * data;

    PlanetSurface();
    PlanetSurface(Json::Value root, Planet * p);

    void drawTile(Tile t, olc::PixelGameEngine * e, CamParams trx);
    void draw(olc::PixelGameEngine * e, CamParams trx);
	void mouseOver(int x, int y, bool mouseClicked, bool mousePressed, CamParams trx);
    olc::Pixel getTint(int x, int y);
};

#endif
