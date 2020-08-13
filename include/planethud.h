#ifndef __PLANETHUD_H
#define __PLANETHUD_H
#include "olcPixelGameEngine.h"
#include "helperfunctions.h"
#include <string>

class Tile;
class PlanetSurface;
class PlanetData;

class DropdownMenuItem {
public:
	std::string text;
	std::function<void()> click;
	olc::vf2d pos;
	olc::vf2d offset;
	DropdownMenuItem(std::string text, std::function<void()> ptr);
	void draw(olc::PixelGameEngine * e, CamParams trx);
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
	void registerItem(DropdownMenuItem item);
};

class PlanetHUD {
private:
	int stone;
	int wood;
	int population;
	Tile * selectedTile = nullptr;
	DropdownMenu * ddmenu = nullptr;
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