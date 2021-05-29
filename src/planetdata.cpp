#include "planetdata.h"
#include "common/enums.h"
#include "tile.h"
#include "olcPixelGameEngine.h"
#include "planetsurface.h"
#include "network.h"
#include <chrono>
#include <string>
#include <iostream>
//BUG: Tasks dont change if selected tile has a task and finishes the task
class PlanetSurface;

PlanetData::PlanetData() {}
PlanetData::PlanetData(PlanetSurface * surface, Json::Value root) {
	this->surface = surface;
	stats = getResourcesFromJson(root["resources"]);
}

std::string pad(std::string str, int n = 2, char chr = '0') {
	return std::string(n - str.size(), chr) + str;
}

void PlanetData::draw(olc::PixelGameEngine * e, CamParams trx) {
	for (Timer &t : timers) {
		olc::vf2d pos = t.target->getTextureCoordinates(trx);
		olc::vf2d offset = {32 * trx.zoom, 64 * trx.zoom}; //TODO fractions of texture size not hardcoded values
		std::string txt = std::to_string((int)(t.time / 60)) + ":" + pad(std::to_string((int)(t.time) % 60));
		e->DrawStringDecal(pos + offset, txt, olc::WHITE, {trx.zoom * 3, trx.zoom * 3});
	}
}

bool PlanetData::dispatchTask(TaskType type, Tile * target) {
	std::cout << "Task dispatch: type: " << (int)type << ": " << "x " << target->x << " y " << target->y << "\n";
	sendUserAction(target, type);
	return false;
}

void PlanetData::updateTimers(float elapsedTime) {
    for (Timer &t : timers) {
        t.time -= elapsedTime;
    }
    timers.erase(std::remove_if(timers.begin(), timers.end(), [](const Timer& t) {return t.time < 0; } ), timers.end());
}

std::vector<TaskType> PlanetData::getPossibleTasks(Tile * target) {
	std::vector<TaskType> v;
	if (isTree(target->type)) {
		v.push_back(TaskType::FELL_TREE);
	}
	if (isMineral(target->type)) {
		v.push_back(TaskType::MINE_ROCK);
	}
	if (isClearable(target->type)) {
		v.push_back(TaskType::CLEAR);
	}
	if (target->type == TileType::GRASS) {
		v.push_back(TaskType::PLANT_TREE);
		v.push_back(TaskType::BUILD_HOUSE);
		v.push_back(TaskType::BUILD_FARM);
		v.push_back(TaskType::BUILD_GREENHOUSE);
		v.push_back(TaskType::BUILD_WATERPUMP);
		v.push_back(TaskType::BUILD_MINE);
		v.push_back(TaskType::BUILD_BLASTFURNACE);
	}
	return v;
}
