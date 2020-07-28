#ifndef __PLANETHUD_H
#define __PLANETHUD_H
#include "olcPixelGameEngine.h"
#include "helperfunctions.h"
#include <string>

class Tile;
class PlanetSurface;
class PlanetData;

struct DropdownMenuItem {
	std::string text;
	void (*click)(DropdownMenuItem * self);
	inline DropdownMenuItem(std::string text, void (*ptr)(DropdownMenuItem * self)) {
		this->text = text;
		this->click = ptr;
	}
};

class DropdownMenu {
public:
	bool open = false;
	std::string text;
	olc::vf2d pos;
	std::vector<DropdownMenuItem> items;
	DropdownMenu(olc::vf2d pos, std::string text);
	void draw(olc::PixelGameEngine * e, CamParams trx);
	bool click(olc::vi2d pos, CamParams trx);
};

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