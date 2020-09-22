#include "network.h"

#include <iostream>
#include <chrono>
#include "client.h"
#include "planet.h"
#include "planetsurface.h"
#include "sector.h"
#include "star.h"
#include "tile.h"

struct SurfaceLocator {
	char planetPos;
	char starPos;
	int sectorX;
	int sectorY;
};

SurfaceLocator getSurfaceLocatorFromJson(Json::Value root) {
	int secX, secY;
    char starPos, planetPos;
	secX = root.get("secX", 0).asInt();
	secY = root.get("secY", 0).asInt();
	starPos = root.get("starPos", 0).asInt();
	planetPos = root.get("planetPos", 0).asInt();
	return {planetPos, starPos, secX, secY};
}

void getJsonFromSurfaceLocator(SurfaceLocator loc, Json::Value& root) {
	root["secX"] = loc.sectorX;
	root["secY"] = loc.sectorY;
	root["starPos"] = loc.starPos;
	root["planetPos"] = loc.planetPos;
}

PlanetSurface * getSurfaceFromJson(Json::Value root, SectorCache * cache) {
	SurfaceLocator loc = getSurfaceLocatorFromJson(root);
	Sector * sec = cache->getSectorAt(loc.sectorX, loc.sectorY);
	if (loc.starPos < sec->numStars) {
		Star * s = &sec->stars[loc.starPos];
		if (loc.planetPos < s->num) {
			Planet * p = &s->planets[loc.planetPos];
			PlanetSurface * surf = p->surface;
			return surf;
		} else {
			return nullptr;
		}
	} else {
		return nullptr;
	}
}

void doUpdate(Json::Value root, SectorCache * cache) {
	PlanetSurface * surf = getSurfaceFromJson(root, cache);
	surf->tiles[root.get("y", 0).asInt() * surf->rad * 2 + root.get("x", 0).asInt()].type = (TileType)root["to"].asInt();
	//Tile * a = &surf->tiles[root.get("y", 0).asInt() * surf->radius * 2 + root.get("x", 0).asInt()];
}

void handleNetwork(tcp::socket * sock, SectorCache * cache) {
    while (true) {
		Json::Value totalJSON;
        std::unique_lock<std::mutex> lk(netq_mutex);
        if (netq.wait_for(lk, std::chrono::seconds(1)) == std::cv_status::timeout) {
			totalJSON["requests"][0]["request"] = "keepAlive";
		}
        lk.unlock();
        if (netThreadStop) { return; }

        int numRequests = netRequests.size();
        std::unique_lock<std::mutex> lock(netq_mutex);
        while (netRequests.size() > 0) {
            Json::Value n = netRequests.back();
            netRequests.pop_back();
            totalJSON["requests"].append(n);
            //std::cout << "Net thread got request " << n << "\n";
        }
        lock.unlock();

        asio::streambuf buf;
        asio::error_code error;
        Json::StreamWriterBuilder builder;
        builder["indentation"] = "";
        const std::string output = Json::writeString(builder, totalJSON);

        asio::write(*sock, asio::buffer(output + "\n"), error);
        /*size_t len = */asio::read_until(*sock, buf, "\n");
        std::istream is(&buf);
        std::string line;
        std::getline(is, line);
        if (error && error != asio::error::eof) {
            throw asio::system_error(error);
        }

        Json::Value root = makeJSON(line);

        for (int i = 0; i < numRequests; i++) {
            if (totalJSON["requests"][i]["request"] == "getSector") {
                Sector s(root["results"][i]["result"]);
                //TODO read status
                cache->setSectorAt(totalJSON["requests"][i]["x"].asInt(), totalJSON["requests"][i]["y"].asInt(), s);
            } else if (totalJSON["requests"][i]["request"] == "getSurface") {
            	Sector * s = cache->getSectorAt(totalJSON["requests"][i]["secX"].asInt(), totalJSON["requests"][i]["secY"].asInt());
            	Star * star= &s->stars[totalJSON["requests"][i]["starPos"].asInt()];
            	Planet * p = &star->planets[totalJSON["requests"][i]["planetPos"].asInt()];
            	PlanetSurface * surf = new PlanetSurface(root["results"][i]["result"], p);
            	p->surface = surf;
            	//TODO read status
            } else if (totalJSON["requests"][i]["request"] == "changeTile") {

			}
        }
		if (root["updates"].isArray()) {
			for (int j = 0; j < (signed)root["updates"].size(); j++) {
				doUpdate(root["updates"][j], cache);
			}
		}
    }
}
