#include "planethud.h"
#include "planet.h"
#include "planetsurface.h"
#include "helperfunctions.h"
#include "planetdata.h"
#include "olcPixelGameEngine.h"
#include "client.h"
#include "sprites.h"

DropdownMenu::DropdownMenu(olc::vf2d pos, std::string text) {
	this->pos = pos;
	this->text = text;
}

void DropdownMenu::draw(olc::PixelGameEngine * e, CamParams trx) {
	
}

bool DropdownMenu::click(olc::vi2d pos, CamParams trx) {
	
}


PlanetHUD::PlanetHUD() {}
PlanetHUD::PlanetHUD(PlanetSurface * parent, PlanetData * data) {
	this->parent = parent;
	this->data = data;
}

void PlanetHUD::draw(olc::PixelGameEngine * e, CamParams trx) {
	e->DrawStringDecal({0, 30}, "Population: " + std::to_string(this->population), olc::WHITE);
	if (this->selectedTile != nullptr) {
		olc::vf2d texturepos = this->selectedTile->getTextureCoordinates(trx);
		//e->FillRectDecal(texturepos - olc::vf2d(128, 0), {128, 10}, olc::WHITE);
		e->DrawDecal(texturepos - olc::vf2d(128, 0), UIComponents["menu_closed"].decal);
		e->DrawStringDecal(texturepos - olc::vf2d(128, 0) + UIComponents["menu_closed"].textPos, "Building", olc::BLACK);
	}
}

bool PlanetHUD::mousePressed(int x, int y, CamParams trx) {
	if (this->selectedTile != nullptr) {
		olc::vf2d texturepos = this->selectedTile->getTextureCoordinates(trx);
		olc::vf2d rectPos = texturepos - olc::vf2d(128, 0);
		if (x - rectPos.x <= 100 &&
			x - rectPos.x >= 0   &&
			y - rectPos.y <= 17  &&
			y - rectPos.y >= 0  ) {
				return true;
		}
	}
	return false;
}

void PlanetHUD::showClickMenu(Tile * t) {
	if (this->selectedTile != nullptr) {
		this->selectedTile->selected = false;
	}
	
	this->selectedTile = t;
	t->selected = true;
}

void PlanetHUD::mouseNotClickedOnAnything(int x, int y) {
	if (this->selectedTile != nullptr) {
		this->selectedTile->selected = false;
		this->selectedTile = nullptr;
	}
}