#ifndef __PLANETDATA_H
#define __PLANETDATA_H
#include <vector>
#include <jsoncpp/json/json.h>
#include "olcPixelGameEngine.h"
#include "helperfunctions.h"
#include "common/enums.h"
#include "common/stats.h"

class PlanetSurface;
struct Tile;

struct Timer {
	Tile * target;
	double time;
};

class PlanetData {
public:
	PlanetSurface * surface;
	std::vector<Timer> timers;
	Stats stats;
	
	PlanetData();
	PlanetData(PlanetSurface * surface, Json::Value root);

	void updateTimers(float elapsedTime);
	std::vector<TaskType> getPossibleTasks(Tile * target);
	bool dispatchTask(TaskType type, Tile * target);
	void draw(olc::PixelGameEngine * e, CamParams trx);
};

#endif
