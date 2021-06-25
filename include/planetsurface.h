#ifndef __PLANETSURFACE_H
#define __PLANETSURFACE_H

#include <jsoncpp/json/json.h>

#include "helperfunctions.h"
#include "olcPixelGameEngine.h"
#include "planethud.h"
//#include "planet.h"

class Planet;

class PlanetSurface {
public:
    std::vector<Tile> tiles;
	int rad;
	int pos;
	int lastSelectX = 0;
	int lastSelectY = 0;
	Tile *selectedTile = nullptr;
    bool generated = false;
    bool requested = false;
	Planet * parent;
	PlanetHUD * hud;
	PlanetData * data;

    PlanetSurface();
    PlanetSurface(Json::Value root, Planet * p);

    void drawTile(Tile t, olc::PixelGameEngine * e, CamParams trx);
    void draw(olc::PixelGameEngine * e, CamParams &trx);
	void mouseOver(int x, int y, bool mouseClicked, bool mousePressed, bool rightClicked, CamParams &trx);
	void updateDirectionalTiles();
    olc::Pixel getTint(int x, int y);
};

#endif
