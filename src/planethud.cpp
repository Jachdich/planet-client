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
	e->DrawDecal(this->pos + offset, comp.decal);
	e->DrawStringDecal(this->pos + comp.textPos + offset, this->text, olc::BLACK);
}

DropdownMenu::DropdownMenu(olc::vf2d pos, std::string text) {
	this->pos = pos;
	this->text = text;
}

void DropdownMenu::registerItem(DropdownMenuItem item) {
	item.pos = this->pos;
	if (this->items.size() > 0) {
		item.offset = this->items[this->items.size() - 1].offset + olc::vf2d(0, 17);
	} else {
		item.offset = olc::vf2d(0, 17);
	}
	this->items.push_back(item);
}

void DropdownMenu::draw(olc::PixelGameEngine * e, CamParams trx) {
	UIComponent component = this->open ? UIComponents["menu_open"] : UIComponents["menu_closed"];
	e->DrawDecal(pos, component.decal);
	e->DrawStringDecal(pos + component.textPos, this->text, olc::BLACK);
	if (this->open) {
		for (DropdownMenuItem item : this->items) {
			item.draw(e, trx);
		}
	}
}

bool DropdownMenu::click(olc::vf2d screenPos, CamParams trx) {
	olc::vf2d rectPos = pos;
	olc::vd2d delta = screenPos - rectPos;
	if (delta.x <= 100 &&
		delta.x >= 0   &&
		delta.y <= 17  &&
		delta.y >= 0  ) {
		this->open = !this->open;
		return true;
	}

	//check X value since all items are the same width
	if (delta.x <= 100 && delta.x >= 0 && this->open) {
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
    e->DrawStringDecal({0, 30}, "Stone: " + std::to_string(this->data->stats.stone), olc::WHITE);
    e->DrawStringDecal({0, 40}, "Wood : " + std::to_string(this->data->stats.wood),  olc::WHITE);
    /*
	e->DrawStringDecal({0, 30}, "Population: " + std::to_string(this->data->people.size()), olc::WHITE);
	int idlePeople = 0;
	for (Person &p : this->data->people) {
		if (p.task.isNone && p.job.isNone) {
			idlePeople++;
		}
	}
	e->DrawStringDecal({0, 40}, "Population Idle: " + std::to_string(idlePeople), olc::WHITE);
    */
	if (this->ddmenu != nullptr) {
		this->ddmenu->draw(e, trx);
	}
	if (this->popupMessage != "") {
	    UIComponent component = UIComponents["error_popup"];
	    olc::vi2d pos = {WIDTH / 2 - component.decal->sprite->width / 2, HEIGHT / 2 - component.decal->sprite->height / 2};
	    e->DrawDecal(pos - olc::vi2d{3, 3}, component.decal);
	    e->DrawStringDecal(pos, this->popupMessage, olc::BLACK);
	}
}

void PlanetHUD::showPopup(std::string message) {
	this->popupMessage = message;
}

bool PlanetHUD::mousePressed(int x, int y, CamParams trx) {
	if (this->popupMessage != "") {
	    UIComponent component = UIComponents["error_popup"];
	    olc::vi2d size = {component.decal->sprite->width, component.decal->sprite->height};
	    olc::vi2d pos = {WIDTH / 2 - size.x / 2, HEIGHT / 2 - size.y / 2};

        olc::vi2d buttonPos = olc::vi2d{177, 19} + pos;
        std::cout << buttonPos.x << ", " << buttonPos.y << "\n";
        olc::vi2d buttonSize = {21, 12};
	    
        if (x > pos.x && x < (pos.x + size.x)
         && y > pos.y && y < (pos.y + size.y)) {
            if (x > buttonPos.x && x < (buttonPos.x + buttonSize.x)
             && y > buttonPos.y && y < (buttonPos.y + buttonSize.y)) {
                this->popupMessage = "";
                std::cout << "e\n";
            }
            return true;
        }
    }
	if (this->ddmenu != nullptr) {
		return this->ddmenu->click(olc::vf2d(x, y), trx);
	}
	return false;
}

void PlanetHUD::showClickMenu(Tile * t) {
	if (this->selectedTile != nullptr) {
		this->selectedTile->selected = false;
	}

	//this->ddmenu = new DropdownMenu(olc::vf2d(128, 0), "Building");
	this->ddmenu = new DropdownMenu(olc::vf2d(128, 8), "Demolition");
	for (TaskType type : this->data->getPossibleTasks(t)) {
		this->ddmenu->registerItem(DropdownMenuItem(getTaskTypeName(type),
		[this, type]() { data->dispatchTask(type, this->selectedTile); }));
	}
	//this->ddmenu->registerItem(DropdownMenuItem("Tree", [this]() { sendChangeTileRequest(TileType::TREE); }));
	//this->ddmenu->registerItem(DropdownMenuItem("Grass", [this]() { sendChangeTileRequest(TileType::GRASS); }));

	this->selectedTile = t;
	t->selected = true;
}

void PlanetHUD::closeClickMenu() {
	if (this->selectedTile != nullptr) {
		//DON'T delete the tile, since it's a pointer to something that still exists and is needed
		this->selectedTile->selected = false;
		this->selectedTile = nullptr;

		//delete the ddmenu since it has no references
		delete this->ddmenu;
		this->ddmenu = nullptr;
	}
}
