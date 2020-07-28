#ifndef __PLANETSURFACE_H
#define __PLANETSURFACE_H

#include <jsoncpp/json/json.h>

#include "helperfunctions.h"
#include "olcPixelGameEngine.h"
#include "planethud.h"
//#include "planet.h"

class Planet;

class Tile {
public:
	int type;
	int z;
	bool hovered = false;
	bool selected = false;
	int x,y;
	Tile(int type, int z, int x, int y);
	olc::vf2d getTextureCoordinates(CamParams trx);
};

class PlanetSurface {
public:
    std::vector<Tile> tiles;
	int radius;
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
    olc::Pixel getTint(int x, int y, Tile * t);

};

#endif
