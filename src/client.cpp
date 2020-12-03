
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "FastNoise.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <math.h>
#include <string>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <asio.hpp>
#include <thread>
#include <condition_variable>

#include "planet.h"
#include "star.h"
#include "sector.h"
#include "sectorcache.h"
#include "game.h"
#include "client.h"

using asio::ip::tcp;
//TODO
//planet noise generation settings
//tidally lock planets/have planets rotate
//star colours/textures
//planets go offscreen
//move around on starsystem view
//starsystem view scrolls on star view

FastNoise noiseGen;
std::mutex cache_mutex;
bool debugMode = false;
Game * app;

int WIDTH = 960;
int HEIGHT = 540;
//int WIDTH = 400;
//int HEIGHT = 240;

int main(int argc, char ** argv) {
    noiseGen.SetNoiseType(FastNoise::Simplex);
	app = new Game(argc, argv);
	if (app->Construct(WIDTH, HEIGHT, 2, 2))
		app->Start();
    app->destruct();
	return 0;
}
