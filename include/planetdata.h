#ifndef __PLANETDATA_H
#define __PLANETDATA_H
#include <vector>
#include <jsoncpp/json/json.h>
#include "olcPixelGameEngine.h"
#include "helperfunctions.h"
#include "common/enums.h"

class PlanetSurface;
struct Tile;

struct Timer {
	TaskType type;
	Tile * target;
	double time;
};

class PlanetData {
public:
	PlanetSurface * surface;
	bool threadStopped = false;
	long lastTimeStamp;
	PlanetData();
	PlanetData(PlanetSurface * surface);
	std::vector<TaskType> getPossibleTasks(Tile * target);
	bool dispatchTask(TaskType type, Tile * target);
	void draw(olc::PixelGameEngine * e, CamParams trx);
	void tick();
	void runLogic();
	void stopThread();
	Json::Value toJSON();
	void fromJSON(Json::Value json, PlanetSurface * surface);
};

#endif
