#include "planethud.h"
#include "planet.h"
#include "planetsurface.h"
#include "helperfunctions.h"
#include "planetdata.h"
#include "olcPixelGameEngine.h"
#include "client.h"

#define TEXTPADDING 1

PlanetHUD::PlanetHUD() {}
PlanetHUD::PlanetHUD(PlanetSurface * parent, PlanetData * data) {
	this->parent = parent;
	this->data = data;
}

void PlanetHUD::draw(olc::PixelGameEngine * e, CamParams trx) {
	e->SetDrawTarget(HUDLayer);
	e->DrawStringDecal({0, 30}, "Population: " + std::to_string(this->population), olc::WHITE);
	if (this->selectedTile != nullptr) {
		olc::vf2d texturepos = this->selectedTile->getTextureCoordinates(trx);
		e->FillRect(texturepos - olc::vf2d(128, 0), {128, 10}, olc::WHITE);
		e->DrawString(texturepos - olc::vf2d(128 - TEXTPADDING, 0 - TEXTPADDING), "Building", olc::BLACK);
	}
	e->SetDrawTarget(renderingLayer);
}

bool PlanetHUD::mousePressed(int x, int y, CamParams trx) {
	if (this->selectedTile != nullptr) {
		olc::vf2d texturepos = this->selectedTile->getTextureCoordinates(trx);
		olc::vf2d rectPos = texturepos - olc::vf2d(128, 0);
		if (x - rectPos.x <= 128 &&
			x - rectPos.x >= 0   &&
			y - rectPos.y <= 10  &&
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