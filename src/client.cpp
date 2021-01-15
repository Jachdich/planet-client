
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
#include <filesystem>

#include "planet.h"
#include "star.h"
#include "sector.h"
#include "sectorcache.h"
#include "game.h"
#include "client.h"

using asio::ip::tcp;

FastNoise noiseGen;
std::mutex cache_mutex;
bool debugMode = false;
Game * app;
olc::vi2d pixelsPerPixel;
Json::Value configJson;

uint16_t WIDTH = 960;
uint16_t HEIGHT = 540;

/*
#if defined _WIN32 || defined _WIN64
	//windows
	#define ABSOLUTE_CONFIG_DIR ""
#endif

#if defined unix || defined __unix || defined __unix__

#endif

#if defined __APPLE__ || defined __MACH__

#endif

#if defined __linux__ || defined linux || defined __linux

#endif
*/

void saveConfig() {
	//step 1: look in current directory
	std::ofstream f("options.json", std::ios::out);
	asio::error_code err;
    Json::StreamWriterBuilder writeBuilder;
    const std::string output = Json::writeString(writeBuilder, configJson);
	f << output;
	f.close();
}

void saveDefaultConfig(std::string path) {
	configJson["fullscreen"] = true;
    configJson["resolutionX"] = 960;
    configJson["resolutionY"] = 540;
    configJson["scale"] = 2;
	saveConfig();
}

void loadConfigFrom(std::string path) {
	std::ifstream f(path + "/options.json");
	
	std::string content((std::istreambuf_iterator<char>(f)), (std::istreambuf_iterator<char>()));
	
    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();

    std::string errors;

    bool parsingSuccessful = reader->parse(
        content.c_str(),
        content.c_str() + content.size(),
        &configJson,
        &errors
    );
    delete reader;

    if (!parsingSuccessful) {
        saveDefaultConfig(path);
    }

    f.close();
}


void loadConfig() {
	//step 1: look in current directory
	std::ifstream f("options.json");

	if (f.good()) {
		loadConfigFrom(".");
	} else {
		//TODO this won't work if checking for another position
		saveDefaultConfig(".");
	}
	
	f.close();

	//todo: is this a good idea?
	/*

	//step 2: look in OS specific path (absolute)
	ifstream f2(ABSOLUTE_CONFIG_DIR.c_str());
	if (f2.good()) {
		loadConfigFrom(ABSOLUTE_CONFIG_DIR);
	}

	//step 3: if neither exist, create OS specific absolute path
	if (!std::filesystem::make_directories(ABSOLUTE_CONFIG_DIR)) {
		//what the fuck?
		return;
	}

	*/
}

int main(int argc, char ** argv) {
	loadConfig();
	WIDTH = configJson["resolutionX"].asUInt();
	HEIGHT = configJson["resolutionY"].asUInt();
	int32_t scale = configJson["scale"].asInt();
	bool fullScreen = configJson["fullscreen"].asBool();

	pixelsPerPixel = {scale, scale};
	
    noiseGen.SetNoiseType(FastNoise::Simplex);
	app = new Game(argc, argv);
	if (app->Construct(WIDTH, HEIGHT, scale, scale, fullScreen))
		app->Start();
    app->destruct();
	return 0;
}
