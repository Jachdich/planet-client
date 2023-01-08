#include "planethud.h"
#include "tile.h"
#include "planet.h"
#include "helperfunctions.h"
#include "planetdata.h"
#include "olcPixelGameEngine.h"
#include "client.h"
#include "sprites.h"
#include "star.h"
#include "sector.h"
#include "helperfunctions.h"

#include <iostream>
//#include <functional>
#include <jsoncpp/json/json.h>
//#include <math>

#define NUM_MENUS 2

DropdownMenuItem::DropdownMenuItem(std::string text, std::function<void(bool)> ptr) {
    this->text = text;
    this->click = ptr;
}

void DropdownMenuItem::draw(olc::PixelGameEngine * e, CamParams &trx) {
    olc::vd2d pos = {this->pos.x * trx.zoom + (int)trx.tx, this->pos.y * trx.zoom + (int)trx.ty};
    UIComponent comp = UIComponents["menu_item"];
    e->DrawDecal(pos + offset * trx.zoom, comp.decal, {trx.zoom, trx.zoom});
    e->DrawStringDecal((this->pos + offset + comp.textPos) * trx.zoom + olc::vi2d{(int)trx.tx, (int)trx.ty}, this->text, olc::BLACK, {trx.zoom, trx.zoom});
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

void DropdownMenu::draw(olc::PixelGameEngine * e, CamParams &trx) {
    UIComponent component = this->open ? UIComponents["menu_open"] : UIComponents["menu_closed"];
    olc::vd2d pos = {this->pos.x * trx.zoom + trx.tx, this->pos.y * trx.zoom + trx.ty};
    e->DrawDecal(pos, component.decal, {trx.zoom, trx.zoom});
    e->DrawStringDecal((this->pos + component.textPos) * trx.zoom + olc::vi2d{(int)trx.tx, (int)trx.ty}, this->text, olc::BLACK, {trx.zoom, trx.zoom});
    if (this->open) {
        for (DropdownMenuItem item : this->items) {
            item.draw(e, trx);
        }
    }
}

bool DropdownMenu::click(olc::vf2d screenPos, bool right, CamParams &trx) {
    olc::vd2d rectPos = {this->pos.x * trx.zoom + trx.tx, this->pos.y * trx.zoom + trx.ty};

    olc::vd2d delta = screenPos - rectPos;
    if (delta.x <= 100 * trx.zoom &&
        delta.x >= 0   * trx.zoom &&
        delta.y <= 17  * trx.zoom &&
        delta.y >= 0   * trx.zoom) {
        this->open = !this->open;
        return true;
    }

    //check X value since all items are the same width
    if (delta.x <= 160 * trx.zoom  && delta.x >= 0 && this->open) {
        int componentIndex = floor(delta.y / (17 * trx.zoom)) - 1; //-1 for the menu header itself
        if ((unsigned long int)componentIndex >= items.size()) {
            return false;
        }

        this->items[componentIndex].click(right);
        return true;
    }

    return false;
}

PlanetHUD::PlanetHUD() {}

void PlanetHUD::draw(olc::PixelGameEngine * e, CamParams &trx, PlanetData *data, Tile *selectedTile) {
    float n = 10;
    for (int r = 0; r < NUM_RESOURCES; r++) {
        if (r == RES_PEOPLE_IDLE) continue;
        ResourceValue v = data->stats.values[r];
        e->DrawDecal({2, n + 12}, icons[std::string(res_names[r])]);
        if (r == RES_PEOPLE) {
            e->DrawStringDecal({16, n += 12}, std::to_string((int)data->stats.values[RES_PEOPLE_IDLE].value) + "/" + std::to_string((int)v.value) + "/" + std::to_string((int)v.capacity), olc::WHITE);
        } else {
            e->DrawStringDecal({16, n += 12}, std::to_string((int)v.value) + "/" + std::to_string((int)v.capacity), olc::WHITE);
        }
    }

    n = 0;
    float xpos = WIDTH - 256;
    if (selectedTile != nullptr) {
        olc::Pixel col = selectedTile->tint;
        TileMinerals minerals = getTileMinerals(col.r << 16 | col.g << 8 | col.b);
        e->DrawStringDecal({xpos, n += 10}, "Selected tile X: " + std::to_string(selectedTile->x) + 
                                                         " Y: " + std::to_string(selectedTile->y) +
                                                         " Z: " + std::to_string(selectedTile->z));

        e->DrawStringDecal({xpos, n += 10}, "Type:       " + std::string(getTileTypeName(selectedTile->type)));
        e->DrawStringDecal({xpos, n += 10}, "Colour:     " + toHexString("#", selectedTile->tint));
        e->DrawStringDecal({xpos, n += 10}, "Iron:       " + std::to_string(minerals.iron * 100) + "%");
        e->DrawStringDecal({xpos, n += 10}, "Copper:     " + std::to_string(minerals.copper * 100) + "%");
        e->DrawStringDecal({xpos, n += 10}, "Aluminium:  " + std::to_string(minerals.aluminium * 100) + "%");
        e->DrawStringDecal({xpos, n += 10}, "Sand:       " + std::to_string(minerals.sand * 100) + "%");
        if (selectedTile->errMsg != "") {
            e->DrawStringDecal({xpos, n += 20}, "Tile Error: " + selectedTile->errMsg);
        }
        
        
    } else {
        e->DrawStringDecal({xpos, n += 10}, "No tile selected");
    }

    if (this->selectedAction != TASK_NONE) {
        e->DrawStringDecal({10, 0}, "Selected task: " + std::string(getTaskTypeName(this->selectedAction)) + " (<esc> to finish)");
    }

    if (this->ddmenu != nullptr) {
        for (size_t i = 0; i < NUM_MENUS; i++) {
            this->ddmenu[i].draw(e, trx);
        }
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

bool PlanetHUD::mousePressed(int x, int y, bool right, CamParams &trx) {
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
        for (size_t i = 0; i < NUM_MENUS; i++) {
            bool clicked = this->ddmenu[i].click(olc::vf2d(x, y), right, trx);
            if (clicked) return true;
        }
    }
    return false;
}

void PlanetHUD::showClickMenu(Tile * t, PlanetData *data) {
    if (this->selectedTile != nullptr) {
        this->selectedTile->selected = false;
    }

    this->ddmenu = new DropdownMenu[2];
    ddmenu[0] = DropdownMenu(t->getTextureCoordinates() - olc::vf2d(32, 32), "Building");
    ddmenu[1] = DropdownMenu(t->getTextureCoordinates() - olc::vf2d(32 + 128, 32), "Demolition");

    std::vector<TaskType> v;
    
    if (isTree(t->type)) {
        v.push_back(TASK_FELL_TREE);
    } else if (isMineral(t->type)) {
        v.push_back(TASK_MINE_ROCK);
    } else if (t->type != TILE_GRASS) {
        v.push_back(TASK_CLEAR);
    }
    
    if (t->type == TILE_GRASS) {
        for (TaskType type : {TASK_PLANT_TREE, TASK_BUILD_HOUSE, TASK_BUILD_FARM,
                 TASK_BUILD_GREENHOUSE, TASK_BUILD_WATERPUMP, TASK_BUILD_MINE,
                 TASK_BUILD_BLASTFURNACE, TASK_BUILD_FORESTRY, TASK_BUILD_CAPSULE,
                 TASK_BUILD_WAREHOUSE, TASK_BUILD_ROAD, TASK_BUILD_PIPE,
                 TASK_BUILD_CABLE, TASK_BUILD_WAREHOUSE}) {
            this->ddmenu[0].registerItem(DropdownMenuItem(getTaskTypeName(type),
            [this, type, data](bool right) { 
                if (right) {
                    if (this->selectedAction == type) {
                        this->selectedAction = TASK_NONE;
                    } else {
                        this->selectedAction = type;
                    }
                } else {
                    data->dispatchTask(type, this->selectedTile);
                }
            }));
        }
    }

    for (TaskType type : v) {
        this->ddmenu[1].registerItem(DropdownMenuItem(getTaskTypeName(type),
        [this, type, data](bool right) { 
            if (right) {
                if (this->selectedAction == type) {
                    this->selectedAction = TASK_NONE;
                } else {
                    this->selectedAction = type;
                }
            } else {
                data->dispatchTask(type, this->selectedTile);
            }
        }));
    }

    this->selectedTile = t;
    t->selected = true;
}

void PlanetHUD::closeClickMenu() {
    if (this->selectedTile != nullptr) {
        //DON'T delete the tile, since it's a pointer to something that still exists and is needed
        this->selectedTile->selected = false;
        this->selectedTile = nullptr;

        //delete the ddmenu since it has no references
        delete[] this->ddmenu;
        this->ddmenu = nullptr;
    }
}
