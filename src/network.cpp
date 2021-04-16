#include "network.h"

#include <iostream>
#include <chrono>
#include "client.h"
#include "planet.h"
#include "planetsurface.h"
#include "sector.h"
#include "star.h"
#include "tile.h"
#include "planetdata.h"
#include "common/surfacelocator.h"
#include "common/surfacelocator_test.h"

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
	std::cout << root << "\n\n\n";
    if (root.get("status", 0).asInt() != 0) {
        std::cerr << "Server sent non-zero status: " << root["status"].asInt() << "\n";
        return;
    }
    
    for (uint32_t i = 0; i < root["requests"].size(); i++) {
        Json::Value req = root["requests"][i];
        Json::Value res = root["results"][i];

        if (res["status"].asInt() != 0) {
            ErrorCode e = (ErrorCode)res["status"].asInt();
            switch (e) {
                case ErrorCode::OK:
                    break;
                case ErrorCode::MALFORMED_JSON:
                case ErrorCode::INVALID_REQUEST:
                case ErrorCode::OUT_OF_BOUNDS:
                    std::cerr << "Server sent non-zero status for request '" << req["request"]
                                          << "': " << res["status"].asInt() << "\n";
            	case ErrorCode::NO_PEOPLE_AVAILABLE: {
            	    PlanetSurface * surf = getSurfaceFromJson(req, cache);
            	    surf->hud->showPopup("No people available to\ncomplete action!");
            	    break;
            	}
                case ErrorCode::INSUFFICIENT_RESOURCES: {
                    PlanetSurface * surf = getSurfaceFromJson(req, cache);
                    surf->hud->showPopup("Insufficient resources!");
                    break;
                }
                case ErrorCode::TASK_ALREADY_STARTED: {
                    PlanetSurface * surf = getSurfaceFromJson(req, cache);
                    surf->hud->showPopup("There is already a task\non this tile!");
                    break;
                }
                case ErrorCode::TASK_ON_WRONG_TILE: {
                    PlanetSurface * surf = getSurfaceFromJson(req, cache);
                    surf->hud->showPopup("This task is not available\non this tile!");
                    break;
                }
            }
            continue;
        }
        
        if (req["request"] == "getSector") {
            Sector s(res["result"]);
            cache->setSectorAt(req["x"].asUInt(), req["y"].asUInt(), s);
            
        } else if (req["request"] == "getSurface") {
        	SurfaceLocator loc = getSurfaceLocatorFromJson(req);
        	Sector * sec = cache->getSectorAt(loc.sectorX, loc.sectorY);
        	Star * s = &sec->stars[loc.starPos];
            Planet * p = &s->planets[loc.planetPos];
            PlanetSurface * surf = new PlanetSurface(res["result"], p);
            p->surface = surf;

        }
    }

    if (root.get("serverRequest", "NONE").asString() != "NONE") {
    	if (root["serverRequest"].asString() == "setTimer") {
    	    PlanetSurface * surface = getSurfaceFromJson(root, cache);
            Tile * target = &surface->tiles[root["tile"].asInt()];
            surface->data->timers.push_back(Timer{target, root["time"].asDouble()});
    	}
    	if (root["serverRequest"].asString() == "statsChange") {
    	    PlanetSurface * surface = getSurfaceFromJson(root, cache);
    	    surface->data->stats["wood"] = root["wood"].asInt();
    	    surface->data->stats["stone"] = root["stone"].asInt();
    	    surface->data->stats["people"] = root["people"].asInt();
    	    surface->data->stats["peopleIdle"] = root["peopleIdle"].asInt();
    	}
    	if (root["serverRequest"].asString() == "changeTile") {
    	    PlanetSurface * surface = getSurfaceFromJson(root, cache);
            surface->tiles[root["tilePos"].asInt()].type = (TileType)root["type"].asInt();
    	}
    	
    }
}

void sendUserAction(Tile * target, TaskType task) {
	Json::Value json;
	json["request"] = "userAction";
	json["action"] = (int)task;
	json["x"] = target->x; //TODO WHY IN GODS NAME DO I HAVE TO COMMIT THIS ATROSITY
	json["y"] = target->y; //OF SWAPPING THE X AND Y VALUES? WHAT DARK MAGIC IS GOING ON?

	std::vector<int> x = app->getCurrentPlanetsurfaceLocator();
	json["secX"] = x[0];
	json["secY"] = x[1];
	json["planetPos"] = x[3];
	json["starPos"] = x[2];
	
	app->client.sendRequest(json);
}

void doUpdate(Json::Value root, SectorCache * cache) {
	PlanetSurface * surf = getSurfaceFromJson(root, cache);
	surf->tiles[root.get("y", 0).asInt() * surf->rad * 2 + root.get("x", 0).asInt()].type = (TileType)root["to"].asInt();
	//Tile * a = &surf->tiles[root.get("y", 0).asInt() * surf->radius * 2 + root.get("x", 0).asInt()];
}

ClientNetwork::ClientNetwork() : ssl_ctx(asio::ssl::context::tls), socket(ctx, ssl_ctx) {
    
}

bool ClientNetwork::connect(std::string address, uint16_t port, SectorCache * cache) {
    this->cache = cache;
    asio::error_code ec;
    asio::ip::tcp::resolver resolver(ctx);
    auto endpoint = resolver.resolve(address, std::to_string(port));
    asio::connect(socket.next_layer(), endpoint, ec);
    if(!ec){}
    else{
        std::cout << "Couldnt connect to the server: " << ec.message() << std::endl;
        return false;
    }
    socket.handshake(asio::ssl::stream_base::client);
    readUntil();
    std::thread asioThread = std::thread([&]() {ctx.run();});
    asioThread.detach();
    return true;
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

