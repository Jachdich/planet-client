#ifndef __CLIENT_H
#define __CLIENT_H
#include <string.h>
#include <jsoncpp/json/json.h>
#include <mutex>
#include <condition_variable>
#include "FastNoise.h"
#include "game.h"

extern FastNoise noiseGen;
extern int WIDTH;
extern int HEIGHT;
extern std::mutex cache_mutex;
extern bool debugMode;
extern Game * app;

Json::Value makeJSON(std::string x);
#endif
