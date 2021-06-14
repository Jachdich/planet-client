#ifndef __PLANETHUD_H
#define __PLANETHUD_H
#include "olcPixelGameEngine.h"
#include "helperfunctions.h"
#include "common/enums.h"
#include <string>

class Tile;
class PlanetSurface;
class PlanetData;

class DropdownMenuItem {
public:
	std::string text;
	std::function<void()> click;
	olc::vd2d pos;
	olc::vd2d offset;
	DropdownMenuItem(std::string text, std::function<void()> ptr);
	void draw(olc::PixelGameEngine * e, CamParams trx);
};

class DropdownMenu {
public:
	bool open = false;
	std::string text;
	olc::vd2d pos;
	std::vector<DropdownMenuItem> items;
	DropdownMenu(olc::vf2d pos, std::string text);
	inline DropdownMenu() {}
	void draw(olc::PixelGameEngine * e, CamParams trx);
	bool click(olc::vf2d pos, CamParams trx);
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
	void closeClickMenu();
	void sendChangeTileRequest(TileType to);
	void draw(olc::PixelGameEngine * e, CamParams trx);
	void showClickMenu(Tile * t);
	void showPopup(std::string message);
    std::string popupMessage = "";
};

#endif
