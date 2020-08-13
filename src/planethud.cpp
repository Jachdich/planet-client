#include "planethud.h"
#include "tile.h"
#include "planet.h"
#include "planetsurface.h"
#include "helperfunctions.h"
#include "planetdata.h"
#include "olcPixelGameEngine.h"
#include "client.h"
#include "sprites.h"
#include "star.h"
#include "sector.h"

#include <iostream>
//#include <functional>
#include <jsoncpp/json/json.h>
//#include <math>

DropdownMenuItem::DropdownMenuItem(std::string text, std::function<void()> ptr) {
	this->text = text;
	this->click = ptr;
}

void DropdownMenuItem::draw(olc::PixelGameEngine * e, CamParams trx) {
	UIComponent comp = UIComponents["menu_item"];
	e->DrawDecal(trx.toScreen(this->pos) + offset, comp.decal);
	e->DrawStringDecal(trx.toScreen(this->pos) + comp.textPos + offset, this->text, olc::BLACK);
}

DropdownMenu::DropdownMenu(olc::vf2d pos, std::string text) {
	this->pos = pos;
	this->text = text;
}

void DropdownMenu::registerItem(DropdownMenuItem item) {
	item.pos = this->pos;
	item.offset = olc::vf2d(0, 17);
	this->items.push_back(item);
}

void DropdownMenu::draw(olc::PixelGameEngine * e, CamParams trx) {
	UIComponent component = this->open ? UIComponents["menu_open"] : UIComponents["menu_closed"];
	e->DrawDecal(trx.toScreen(pos), component.decal);
	e->DrawStringDecal(trx.toScreen(pos) + component.textPos, this->text, olc::BLACK);
	if (this->open) {
		for (DropdownMenuItem item : this->items) {
			item.draw(e, trx);
		}
	}
}

bool DropdownMenu::click(olc::vi2d screenPos, CamParams trx) {
	olc::vf2d rectPos = trx.toScreen(pos);
	olc::vd2d delta = screenPos - rectPos;
	if (delta.x <= 100 &&
		delta.x >= 0   &&
		delta.y <= 17  &&
		delta.y >= 0  ) {
		this->open = !this->open;
		return true;
	}

	//check X value since all items are the same width
	if (delta.x <= 100 && delta.x >= 0) {
		int componentIndex = floor(delta.y / 17) - 1; //1 for the menu header itself
		if ((unsigned long int)componentIndex >= items.size()) {
			return false;
		}

		this->items[componentIndex].click();
		return true;
	}

	return false;
}

PlanetHUD::PlanetHUD() {}
PlanetHUD::PlanetHUD(PlanetSurface * parent, PlanetData * data) {
	this->parent = parent;
	this->data = data;
}

void PlanetHUD::draw(olc::PixelGameEngine * e, CamParams trx) {
	e->DrawStringDecal({0, 30}, "Population: " + std::to_string(this->population), olc::WHITE);
	if (this->ddmenu != nullptr) {
		this->ddmenu->draw(e, trx);
	}
}

bool PlanetHUD::mousePressed(int x, int y, CamParams trx) {
	if (this->ddmenu != nullptr) {
		return this->ddmenu->click(olc::vf2d(x, y), trx);
	}
	return false;
}

void PlanetHUD::showClickMenu(Tile * t) {
	if (this->selectedTile != nullptr) {
		this->selectedTile->selected = false;
	}

	this->ddmenu = new DropdownMenu(t->getTextureCoordinates() - olc::vf2d(128, 0), "Building");
	std::function<void()> thing = [this]() {
		Json::Value json;
		json["request"] = "changeTile";
		json["x"] = selectedTile->x;
		json["y"] = selectedTile->y;
		selectedTile->type = TileType::TREE;

		std::vector<int> x = app->getCurrentPlanetsurfaceLocator();
		json["planetPos"] = x[3];
		json["starPos"] = x[2];
		json["secX"] = x[0];
		json["secY"] = x[1];

		json["to"] = (int)TileType::TREE;
		std::lock_guard<std::mutex> lock(netq_mutex);
		netRequests.push_back(json);
		netq.notify_all();
	};
	DropdownMenuItem item("Tree", thing);
	this->ddmenu->registerItem(item);
	this->selectedTile = t;
	t->selected = true;
}

void PlanetHUD::mouseNotClickedOnAnything(int x, int y) {
	if (this->selectedTile != nullptr) {
		//DON'T delete the tile, since it's a pointer to something that still exists and is needed
		this->selectedTile->selected = false;
		this->selectedTile = nullptr;

		//delete the ddmenu since it has no references
		delete this->ddmenu;
		this->ddmenu = nullptr;
	}
}