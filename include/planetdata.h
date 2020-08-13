#ifndef __PLANETDATA_H
#define __PLANETDATA_H
#include <vector>

class PlanetSurface;
struct Tile;

struct Task {
	Tile * pos;
	int type;
	int durationLeft;
};

struct Job {
	Tile * pos;
};

struct Person {
	Task * task = nullptr;
	Job  * job  = nullptr;
	int age;
};

class PlanetData {
public:
	std::vector<Person> people;
	std::vector<Task>   toDoTasks;
	std::vector<Task>   doingTasks;
	std::vector<Job>    unassignedJobs;
	std::vector<Job>    assignedJobs;
	PlanetSurface * surface;
	PlanetData();
	PlanetData(PlanetSurface * surface);
};

#endif