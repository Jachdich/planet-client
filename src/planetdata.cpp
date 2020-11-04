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

void PlanetData::tick() {
	long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count();

	long elapsedTime = ms - this->lastTimeStamp;
	double elapsedS = ((double)elapsedTime) / 1000.0;

	std::unique_lock<std::mutex> lk(muxTimers);
	for (Timer &t: timers) {
		t.time -= elapsedS;
		if (t.time <= 0) {
			std::cout << "Timer finished with task " << (int)t.type << "\n";
			switch (t.type) {
				case TaskType::GATHER_MINERALS:
				    t.target->type = TileType::GRASS;
                    this->surface->stats.stone += 1;
				    break;
				case TaskType::CLEAR:
				case TaskType::FELL_TREE:
					t.target->type = TileType::GRASS;
					this->surface->stats.wood += 1;
					break;

				case TaskType::PLANT_TREE:
					t.target->type = TileType::TREE;
					break;
                case TaskType::BUILD_HOUSE:
                    t.target->type = TileType::HOUSE;
                    break;
				default:
					break;
			}
		}
	}
	timers.erase(std::remove_if(timers.begin(), timers.end(),
	[](Timer& t) {
		return t.time <= 0;
	}), timers.end());
	lk.unlock();
	this->lastTimeStamp = ms; //todo take into account the time taken for tick8
}

std::string pad(std::string str, int n = 2, char chr = '0') {
	return std::string(n - str.size(), chr) + str;
}

void PlanetData::draw(olc::PixelGameEngine * e, CamParams trx) {
	/*
	for (Person &p : this->people) {
		if (!p.task.isNone) {
			olc::vf2d pos = p.task.target->getTextureCoordinates(trx);
			olc::vf2d offset = {32 * trx.zoom, 64 * trx.zoom}; //TODO fractions of texture size not hardcoded values
			std::string txt = std::to_string((int)(p.task.durationLeft / 60)) + ":" + pad(std::to_string((int)(p.task.durationLeft) % 60));
			e->DrawStringDecal(pos + offset, txt, olc::WHITE, {trx.zoom * 3, trx.zoom * 3});
		}
	}*/
	std::unique_lock<std::mutex> lk(muxTimers);
	for (Timer &t : timers) {
		olc::vf2d pos = t.target->getTextureCoordinates(trx);
		olc::vf2d offset = {32 * trx.zoom, 64 * trx.zoom}; //TODO fractions of texture size not hardcoded values
		std::string txt = std::to_string((int)(t.time / 60)) + ":" + pad(std::to_string((int)(t.time) % 60));
		e->DrawStringDecal(pos + offset, txt, olc::WHITE, {trx.zoom * 3, trx.zoom * 3});
	}
}

void PlanetData::runLogic() {
	while (!threadStopped) {
		unsigned long long startns = std::chrono::duration_cast< std::chrono::microseconds >(
	    	std::chrono::system_clock::now().time_since_epoch()).count();
		tick();

		unsigned long long endns = std::chrono::duration_cast< std::chrono::microseconds >(
	    	std::chrono::system_clock::now().time_since_epoch()).count();

		std::this_thread::sleep_for(std::chrono::microseconds(100000 - (endns - startns)));
	}
}

void PlanetData::stopThread() {
	this->threadStopped = true;
}

bool hasMaterialsFor(PlanetSurface * surf, TaskType type) {
    switch (type) {
        case TaskType::BUILD_HOUSE:
            if (surf->stats.wood >= 3 && surf->stats.stone >= 6) return true;
            break;
        default:
            return true;
    }
    return false;
}

bool PlanetData::dispatchTask(TaskType type, Tile * target) {
	/*
	for (Person &p : this->people) {
		if (p.job.isNone && p.task.isNone) {
			p.task = Task(type, target);
			return true;
		}
	}*/
	if (!hasMaterialsFor(this->surface, type)) { return false; }
	sendUserAction(target, type,
	[type, target](int time, ErrorCode code) {
		std::cout << time << ", code " << (int)code << "\n";
		Timer timer = {type, target, (double)time};
		std::lock_guard<std::mutex> lk(muxTimers);
		timers.push_back(timer);
	});
	return true;
}

std::vector<TaskType> PlanetData::getPossibleTasks(Tile * target) {
    //TODO This is unintuitive to have it here and also bad because of push_back
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
		v.push_back(TaskType::BUILD_HOUSE);
	}
	return v;
}

Json::Value PlanetData::toJSON() {
	Json::Value res;
	return res;
}

void PlanetData::fromJSON(Json::Value json, PlanetSurface * surface) {
	this->surface = surface;
}
