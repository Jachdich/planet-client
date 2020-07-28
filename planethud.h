#ifndef __PLANETHUD_H
#define __PLANETHUD_H
#include "olcPixelGameEngine.h"
#include "helperfunctions.h"

class Tile;
class PlanetSurface;
class PlanetData;

class PlanetHUD {
private:
	int stone;
	int wood;
	int population;
	Tile * selectedTile = nullptr;
	PlanetSurface * parent;
	PlanetData * data;
public:
	PlanetHUD();
	PlanetHUD(PlanetSurface * parent, PlanetData * data);
	bool mousePressed(int x, int y, CamParams trx);
	void mouseNotClickedOnAnything(int x, int y); //frikkin change the name of this
	void draw(olc::PixelGameEngine * e, CamParams trx);
	void showClickMenu(Tile * t);
};

#endif