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

struct Materials {
	int wood = 0;
	int stone = 0;
	int iron = 0;
};

class PlanetData {
public:
	PlanetSurface * surface;
	PlanetData();
	PlanetData(PlanetSurface * surface);
	std::vector<TaskType> getPossibleTasks(Tile * target);
	bool dispatchTask(TaskType type, Tile * target);
	void draw(olc::PixelGameEngine * e, CamParams trx);
};

#endif
