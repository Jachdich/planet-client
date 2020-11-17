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

/*
COMMUNICATION DIAGRAM

User starts task (e.g. fell tree)
Client: "hi user said fell tree x,y"
Server: "Ok set timer for 30 seconds" OR "Can't do it for X reason"
(timer set on client and server)
(time runs out)
Server sets tile to correct type
Client does likewise
*/

std::unordered_map<int, std::function<void(int, ErrorCode)>> callbacks;
uint32_t fuckin_index_into_callbacks = 0;

void handleNetwork(asio::ip::tcp::socket * sock) {
    while (true) {
		Json::Value totalJSON;
        std::unique_lock<std::mutex> lk(netq_mutex);
        if (netq.wait_for(lk, std::chrono::seconds(1)) == std::cv_status::timeout) {
			totalJSON["requests"][0]["request"] = "keepAlive";
		}
        lk.unlock();
        if (netThreadStop) { return; }

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
    }
}

void handleNetworkPacket(Json::Value root, SectorCache * cache) {
    for (uint32_t i = 0; i < root["requests"].size(); i++) {
        Json::Value req = root["requests"][i];
        Json::Value res = root["results"][i];
        std::cout << req << "\n";
        if (req["request"] == "getSector") {
            Sector s(res["result"]);
            //TODO read status
            cache->setSectorAt(req["x"].asInt(), req["y"].asInt(), s);
        } else if (req["request"] == "getSurface") {
            Sector * s = cache->getSectorAt(req["secX"].asInt(), req["secY"].asInt());
            Star * star= &s->stars[req["starPos"].asInt()];
            Planet * p = &star->planets[req["planetPos"].asInt()];
            PlanetSurface * surf = new PlanetSurface(res["result"], p);
            p->surface = surf;
            //TODO read status
        } else if (req["request"] == "changeTile") {

        } else if (req["request"] == "userAction") {
            callbacks[req["callback"].asInt()](res["time"].asInt(), (ErrorCode)res["status"].asInt());
            callbacks.erase(req["callback"].asInt());
        }
    }
}

void sendUserAction(Tile * target, TaskType task, std::function<void(int, ErrorCode)> callback) {
	Json::Value json;
	json["request"] = "userAction";
	json["action"] = (int)task;
	json["x"] = target->y; //TODO WHY IN GODS NAME DO I HAVE TO COMMIT THIS ATROSITY
	json["y"] = target->x; //OF SWAPPING THE X AND Y VALUES? WHAT DARK MAGIC IS GOING ON?

	std::vector<int> x = app->getCurrentPlanetsurfaceLocator();
	json["planetPos"] = x[3];
	json["starPos"] = x[2];
	json["secX"] = x[0];
	json["secY"] = x[1];
	json["callback"] = fuckin_index_into_callbacks;

	callbacks[fuckin_index_into_callbacks] = callback;
	fuckin_index_into_callbacks++;
	std::lock_guard<std::mutex> lock(netq_mutex);
	netRequests.push_back(json);
	netq.notify_all();
}

void sendChangeTileRequest(Tile * target, TileType to) {
	Json::Value json;
	json["request"] = "changeTile";
	json["x"] = target->y; //TODO WHY IN GODS NAME DO I HAVE TO COMMIT THIS ATROSITY
	json["y"] = target->x; //OF SWAPPING THE X AND Y VALUES? WHAT DARK MAGIC IS GOING ON?

	std::vector<int> x = app->getCurrentPlanetsurfaceLocator();
	json["planetPos"] = x[3];
	json["starPos"] = x[2];
	json["secX"] = x[0];
	json["secY"] = x[1];

	json["to"] = (int)to;
	std::lock_guard<std::mutex> lock(netq_mutex);
	netRequests.push_back(json);
	netq.notify_all();
}

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

ClientNetwork::ClientNetwork() : socket(ctx) {
    
}

void ClientNetwork::connect(std::string address, uint16_t port, SectorCache * cache) {
    this->cache = cache;
    asio::error_code ec;
    //asio::io_context::work work(ctx);

    asio::ip::tcp::endpoint endpoint(asio::ip::make_address(address, ec), port);
    socket.connect(endpoint, ec);
    /*
    if (!ec) {
        std::cout << "connected\n";
        if (socket.is_open()) {
            std::string req = "{\"requests\": [{\"request\": \"getSector\", \"x\": 0, \"y\": 0}]}\n";
            socket.write_some(asio::buffer(req.data(), req.size()), ec);
            if (ec) {
                std::cout << "Error: " << ec.message() << "\n";
            }
            readUntil();
        }
    } else {
        std::cout << "error: " << ec.message() << "\n";
        ctx.stop();
    }*/
    readUntil();
    std::thread asioThread = std::thread([&]() {ctx.run();});
    std::thread networkThread = std::thread([this]() {handleNetwork(&socket);});
    asioThread.detach();
    networkThread.detach();
    
}

void ClientNetwork::handler(std::error_code ec, size_t bytes_transferred) {
    std::cout << bytes_transferred << "\n";
    if (!ec) {
        std::string request(buf.begin(), buf.begin() + bytes_transferred);
        buf.clear();

        Json::CharReaderBuilder builder;
        Json::CharReader* reader = builder.newCharReader();

        Json::Value root;
        std::string errors;

        bool parsingSuccessful = reader->parse(
            request.c_str(),
            request.c_str() + request.size(),
            &root,
            &errors
        );
        delete reader;

        if (!parsingSuccessful) {
            std::cerr << "Server sent malformed JSON: " << request << ". Full error: " << errors;
            //asio::error_code ign_error;
            //asio::write(sock, asio::buffer("{\"status\": -1}\n"), ign_error);

        } else {
            handleNetworkPacket(root, cache);
        }
        readUntil();
    } else {
        std::cerr << "ERROR: " <<  ec.message() << "\n";
    }
}

void ClientNetwork::readUntil() {
    asio::async_read_until(socket, asio::dynamic_buffer(buf), "\n", [this] (std::error_code ec, std::size_t bytes_transferred) {
        handler(ec, bytes_transferred);
    });
}

