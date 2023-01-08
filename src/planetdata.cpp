#include "planetdata.h"
#include "common/enums.h"
#include "tile.h"
#include "olcPixelGameEngine.h"
#include "network.h"
#include <chrono>
#include <string>
#include <iostream>
//BUG: Tasks dont change if selected tile has a task and finishes the task

#define DEBUG(expr) std::cout << __FILE__ << ":" << __LINE__ << " " << expr

PlanetData::PlanetData() {}
PlanetData::PlanetData(Json::Value root) {
    stats = res_from_json(root["resources"]);
    timers.clear();
}

std::string pad(std::string str, int n = 2, char chr = '0') {
    return std::string(n - str.size(), chr) + str;
}

void PlanetData::draw(olc::PixelGameEngine * e, CamParams &trx) {
    for (Timer &t : timers) {
        olc::vf2d pos = t.target->getTextureCoordinates(trx);
        olc::vf2d offset = {32 * trx.zoom, 64 * trx.zoom}; //TODO fractions of texture size not hardcoded values
        std::string txt = std::to_string((int)(t.time / 60)) + ":" + pad(std::to_string((int)(t.time) % 60));
        e->DrawStringDecal(pos + offset, txt, olc::WHITE, {trx.zoom * 3, trx.zoom * 3});
    }
}

bool PlanetData::dispatchTask(TaskType type, Tile * target) {
    DEBUG("Task dispatch: type: " << (int)type << ": " << "x " << target->x << " y " << target->y << "\n");
    sendUserAction(target, type);
    return false;
}

void PlanetData::updateTimers(float elapsedTime) {
    for (Timer &t : timers) {
        t.time -= elapsedTime;
    }
    timers.erase(std::remove_if(timers.begin(), timers.end(), [](const Timer& t) {return t.time < 0; } ), timers.end());
}

[[deprecated]]
std::vector<TaskType> PlanetData::getPossibleTasks(Tile * target) {
    std::vector<TaskType> v;
    if (isTree(target->type)) {
        v.push_back(TASK_FELL_TREE);
    }
    if (isMineral(target->type)) {
        v.push_back(TASK_MINE_ROCK);
    }
    if (target->type == TILE_GRASS) {
        v.push_back(TASK_PLANT_TREE);
        v.push_back(TASK_BUILD_HOUSE);
        v.push_back(TASK_BUILD_FARM);
        v.push_back(TASK_BUILD_GREENHOUSE);
        v.push_back(TASK_BUILD_WATERPUMP);
        v.push_back(TASK_BUILD_MINE);
        v.push_back(TASK_BUILD_BLASTFURNACE);
        v.push_back(TASK_BUILD_FORESTRY);
        v.push_back(TASK_BUILD_CAPSULE);
        v.push_back(TASK_BUILD_WAREHOUSE);
        v.push_back(TASK_BUILD_ROAD);
        v.push_back(TASK_BUILD_PIPE);
        v.push_back(TASK_BUILD_CABLE);
        v.push_back(TASK_BUILD_POWERSTATION);
    } else {
        v.push_back(TASK_CLEAR);
    }
    return v;
}
