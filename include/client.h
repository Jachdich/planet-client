#ifndef __CLIENT_H
#define __CLIENT_H
#include <string.h>
#include <jsoncpp/json/json.h>
#include <mutex>
#include <condition_variable>
#include "FastNoiseLite.h"
#include "game.h"

extern FastNoiseLite noiseGen;
extern uint16_t WIDTH;
extern uint16_t HEIGHT;
extern std::mutex cache_mutex;
extern bool debugMode;
extern Game * app;
extern olc::vi2d pixelsPerPixel;
extern Json::Value configJson;

Json::Value makeJSON(std::string x);
#endif
