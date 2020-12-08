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

void ClientNetwork::sendRequest(Json::Value request) {
    Json::Value totalJSON;
    totalJSON["requests"].append(request);

    asio::streambuf buf;
    asio::error_code error;
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    const std::string output = Json::writeString(builder, totalJSON);

    asio::write(socket, asio::buffer(output + "\n"), error);
    if (error) {
        std::cout << "ERROR SENDING: " << error.message() << "\n";
    }
}

void handleNetworkPacket(Json::Value root, SectorCache * cache) {
    if (root.get("status", 0).asInt() != 0) {
        std::cerr << "Server sent non-zero status: " << root["status"].asInt() << "\n";
        return;
    }
    
    for (uint32_t i = 0; i < root["requests"].size(); i++) {
        Json::Value req = root["requests"][i];
        Json::Value res = root["results"][i];

        if (res["status"].asInt() != 0) {
            std::cerr << "Server sent non-zero status for request '" << req["request"]
                      << "': " << res["status"].asInt() << "\n";
            continue;
        }
        
        if (req["request"] == "getSector") {
            Sector s(res["result"]);
            cache->setSectorAt(req["x"].asInt(), req["y"].asInt(), s);
            
        } else if (req["request"] == "getSurface") {
            Sector * s = cache->getSectorAt(req["secX"].asInt(), req["secY"].asInt());
            Star * star= &s->stars[req["starPos"].asInt()];
            Planet * p = &star->planets[req["planetPos"].asInt()];
            PlanetSurface * surf = new PlanetSurface(res["result"], p);
            p->surface = surf;

        } else if (req["request"] == "changeTile") {

        } else if (req["request"] == "userAction") {

        }
    }

    if (root.get("serverRequest", "NONE").asString() != "NONE") {
    	if (root["serverRequest"].asString() == "setTimer") {
    		
    	}
    }
}

void sendUserAction(Tile * target, TaskType task) {
	Json::Value json;
	json["request"] = "userAction";
	json["action"] = (int)task;
	json["x"] = target->y; //TODO WHY IN GODS NAME DO I HAVE TO COMMIT THIS ATROSITY
	json["y"] = target->x; //OF SWAPPING THE X AND Y VALUES? WHAT DARK MAGIC IS GOING ON?

	std::vector<int> x = app->getCurrentPlanetsurfaceLocator();
	json["secX"] = x[0];
	json["secY"] = x[1];
	json["planetPos"] = x[3];
	json["starPos"] = x[2];
	
	app->client.sendRequest(json);
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

ClientNetwork::ClientNetwork() : ssl_ctx(asio::ssl::context::tls), socket(ctx, ssl_ctx) {
    
}

void ClientNetwork::connect(std::string address, uint16_t port, SectorCache * cache) {
    this->cache = cache;
    asio::error_code ec;

    asio::ip::tcp::resolver resolver(ctx);
    auto endpoint = resolver.resolve(address, std::to_string(port));
    asio::connect(socket.next_layer(), endpoint);
    socket.handshake(asio::ssl::stream_base::client);

    readUntil();
    std::thread asioThread = std::thread([&]() {ctx.run();});
    asioThread.detach();
}

void ClientNetwork::handler(std::error_code ec, size_t bytes_transferred) {
    std::cout << bytes_transferred << "\n";
    if (!ec) {
    
        std::string request(
                buffers_begin(buf.data()),
                buffers_begin(buf.data()) + (bytes_transferred
                  - 1));

        /*
        buf.commit(bytes_transferred);
        std::istream is(&buf);
        std::string request;
        is >> request;*/
        buf.consume(bytes_transferred);
        readUntil();

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
            std::cerr << "Server sent malformed JSON: '" << request << "'. Full error: " << errors;

        } else {
            handleNetworkPacket(root, cache);
        }

    } else {
        std::cerr << "ERROR: " <<  ec.message() << "\n";
        readUntil();
    }
}

void ClientNetwork::readUntil() {
    asio::async_read_until(socket, buf, '\n', [this] (std::error_code ec, std::size_t bytes_transferred) {
        handler(ec, bytes_transferred);
    });
}

