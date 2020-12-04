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

std::mutex muxTimers;
std::vector<Timer> timers;

PlanetData::PlanetData() {}
PlanetData::PlanetData(PlanetSurface * surface) {
	this->surface = surface;
}

std::string pad(std::string str, int n = 2, char chr = '0') {
	return std::string(n - str.size(), chr) + str;
}

void PlanetData::draw(olc::PixelGameEngine * e, CamParams trx) {
	std::unique_lock<std::mutex> lk(muxTimers);
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

std::vector<TaskType> PlanetData::getPossibleTasks(Tile * target) {
	std::vector<TaskType> v;
	if (isTree(target->type)) {
		v.push_back(TaskType::FELL_TREE);
	}
	if (isMineral(target->type)) {
		v.push_back(TaskType::GATHER_MINERALS);
	}
	if (isClearable(target->type)) {
		v.push_back(TaskType::CLEAR);
	}
	if (target->type == TileType::GRASS) {
		v.push_back(TaskType::PLANT_TREE);
	}
	return v;
}
