#include "planetdata.h"
#include "common/enums.h"
#include "tile.h"
#include "olcPixelGameEngine.h"
#include "planetsurface.h"
#include "network.h"
#include <chrono>
#include <string>

class PlanetSurface;
PlanetData::PlanetData() {}
PlanetData::PlanetData(PlanetSurface * surface) {
	this->surface = surface;
	for (int i = 0; i < 4; i++) {
		this->people.push_back(Person());
	}
}

Person::Person() {

}

void Person::tick(long elapsedMs) {
	double elapsedS = elapsedMs / 1000.0;
	if (!this->task.isNone) {
		this->task.durationLeft -= elapsedS;
		switch (this->task.type) {
			case TaskType::FELL_TREE:
				if (this->task.durationLeft <= 0) {
					this->task.target->type = TileType::GRASS;
					sendChangeTileRequest(this->task.target, TileType::GRASS);
					this->task = Task(); //task(NONE);
				}
				break;

				case TaskType::CLEAR:
				if (this->task.durationLeft <= 0) {
					this->task.target->type = TileType::GRASS;
					sendChangeTileRequest(this->task.target, TileType::GRASS);
					this->task = Task(); //task(NONE);
				}
				break;
			case TaskType::GATHER_MINERALS:
				if (this->task.durationLeft <= 0) {
					this->task.target->type = TileType::GRASS;
					sendChangeTileRequest(this->task.target, TileType::GRASS);
					this->task = Task(); //task(NONE);
				}
				break;
			case TaskType::PLANT_TREE:
				if (this->task.durationLeft <= 0) {
					this->task.target->type = TileType::TREE;
					sendChangeTileRequest(this->task.target, TileType::TREE);
					this->task = Task(); //task(NONE);
				}
				break;
		}
	}
}

void PlanetData::tick() {
	long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count();

	long elapsedTime = ms - this->lastTimeStamp;
	for (Person &p : this->people) {
		p.tick(elapsedTime);
	}

	this->lastTimeStamp = ms; //todo take into account the time taken for tick8
}

std::string pad(std::string str, int n = 2, char chr = '0') {
	return std::string(n - str.size(), chr) + str;
}

void PlanetData::draw(olc::PixelGameEngine * e, CamParams trx) {
	for (Person &p : this->people) {
		if (!p.task.isNone) {
			olc::vf2d pos = p.task.target->getTextureCoordinates(trx);
			olc::vf2d offset = {32 * trx.zoom, 64 * trx.zoom}; //TODO fractions of texture size not hardcoded values
			std::string txt = std::to_string((int)(p.task.durationLeft / 60)) + ":" + pad(std::to_string((int)(p.task.durationLeft) % 60));
			e->DrawStringDecal(pos + offset, txt, olc::WHITE, {trx.zoom * 3, trx.zoom * 3});
		}
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

bool PlanetData::dispatchTask(TaskType type, Tile * target) {
	/*
	for (Person &p : this->people) {
		if (p.job.isNone && p.task.isNone) {
			p.task = Task(type, target);
			return true;
		}
	}*/
	sendUserAction(target, type,
	[](int time, ErrorCode code) {
		std::cout << time << ", code " << (int)code << "\n";
	});
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

Job::Job() {
}

Job::Job(Json::Value json, PlanetSurface * surface) {
	fromJSON(json, surface);
}

Task::Task() {
	isNone = true;
}

Task::Task(Json::Value json, PlanetSurface * surf) {
	fromJSON(json, surf);
}

Task::Task(TaskType type, Tile * target) {
	this->type = type;
	this->target = target;
	switch (type) {
		case TaskType::FELL_TREE:
			durationLeft = 6.0;
			break;
		case TaskType::CLEAR:
			durationLeft = 3.0;
			break;
		case TaskType::GATHER_MINERALS:
			durationLeft = 10.0;
			break;
		case TaskType::PLANT_TREE:
			durationLeft = 5.0;
			break;
	}
	isNone = false;
}

Json::Value Task::toJSON() {
	Json::Value res;
	res["type"] = (int)type;
	res["isNone"] = isNone;
	res["durationLeft"] = durationLeft;
	res["targetX"] = target->x;
	res["targetY"] = target->y;
	return res;
}

void Task::fromJSON(Json::Value res, PlanetSurface * surf) {
	type = (TaskType)res["type"].asInt();
	isNone = res["isNone"].asBool();
	durationLeft = res["durationLeft"].asDouble();
	int x = res["targetX"].asInt(),
		y = res["targetY"].asInt();
	target = &surf->tiles[y * surf->rad + x];
}

Person::Person(Json::Value json, PlanetSurface * surface) {
	fromJSON(json, surface);
}

Json::Value Job::toJSON() {
	Json::Value res;
	res["isNone"] = isNone;
	res["targetX"] = pos->x;
	res["targetY"] = pos->y;
	return res;
}

void Job::fromJSON(Json::Value json, PlanetSurface * surf) {
	isNone = json["isNone"].asBool();
	int x = json["targetX"].asInt(),
		y = json["targetY"].asInt();
	pos = &surf->tiles[y * surf->rad + x];
}

Json::Value Person::toJSON() {
	Json::Value res;
	res["age"] = age;
	res["task"] = task.toJSON();
	res["job"] = job.toJSON();
	return res;
}

void Person::fromJSON(Json::Value json, PlanetSurface * surface) {
	age = json["age"].asInt();
	task = Task(json["task"], surface);
	job = Job(json["job"], surface);
}

Json::Value PlanetData::toJSON() {
	Json::Value res;
	for (Person &p : people) {
		res["people"].append(p.toJSON());
	}
	return res;
}

void PlanetData::fromJSON(Json::Value json, PlanetSurface * surface) {
	for (Json::Value val : json["people"]) {
		people.push_back(Person(val, surface));
	}
	this->surface = surface;
}
