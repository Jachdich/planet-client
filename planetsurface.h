#ifndef __PLANETSURFACE_H
#define __PLANETSURFACE_H

#include <jsoncpp/json/json.h>

#include "helperfunctions.h"
#include "olcPixelGameEngine.h"
//#include "planet.h"

class Planet;

class PlanetSurface {
public:
    std::vector<int> tiles;
	int radius;
	int pos;
	int lastSelectX = 0;
	int lastSelectY = 0;
    bool generated = false;
    bool requested = false;
	Planet * parent;
    
    PlanetSurface();
    PlanetSurface(Json::Value root, Planet * p);

    void drawTile(int ax, int ay, int thing, olc::PixelGameEngine * e, CamParams trx);
    void draw(olc::PixelGameEngine * e, CamParams trx);
	void mouseOver(int x, int y, CamParams trx);
    olc::Pixel getTint(int x, int y);

};

#endif
