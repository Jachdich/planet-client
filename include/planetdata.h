#ifndef __PLANETDATA_H
#define __PLANETDATA_H
#include <vector>
#include <jsoncpp/json/json.h>
#include "olcPixelGameEngine.h"
#include "helperfunctions.h"
#include "common/enums.h"

class PlanetSurface;
struct Tile;

struct Task {
	Tile * target;
	TaskType type;
	double durationLeft;
	bool isNone = true;
	Task();
	Task(Json::Value json, PlanetSurface * surf);
	Task(TaskType type, Tile * target);
	Json::Value toJSON();
	void fromJSON(Json::Value json, PlanetSurface * surf);
};

struct Job {
	Tile * pos;
	bool isNone = true;
	Job();
	Job(Json::Value json, PlanetSurface * surface);
	Json::Value toJSON();
	void fromJSON(Json::Value json, PlanetSurface * surf);
};

struct Person {
	Task task;
	Job job;
	int age;
	void tick(long elapsedTime);
	Person();
	Person(Json::Value json, PlanetSurface * surface);
	Json::Value toJSON();
	void fromJSON(Json::Value json, PlanetSurface * surface);
};

class PlanetData {
public:
	std::vector<Person> people;
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
