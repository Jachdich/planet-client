#include "../include/network.h"

#include <iostream>
#include <chrono>
#include "../include/client.h"
#include "../include/planet.h"
#include "../include/planetsurface.h"
#include "../include/sector.h"
#include "../include/star.h"
#include "../include/tile.h"
#include "../include/planetdata.h"
#include "../include/common/surfacelocator.h"
#include "../include/common/surfacelocator_test.h"

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
    for (uint32_t i = 0; i < root["results"].size(); i++) {
        Json::Value res = root["results"][i];
        std::cout << res << "\n\n";

        if (res["status"].asInt() != 0) {
            int e = res["status"].asInt();
            switch (e) {
                case ERR_OK:
                    break;
                case ERR_MALFORMED_JSON:
                case ERR_INVALID_REQUEST:
                case ERR_OUT_OF_BOUNDS:
                    std::cerr << "Server sent non-zero status for request '" << res["request"]
                                          << "': " << res["status"].asInt() << "\n";
                    break;
                case ERR_INVALID_ACTION: {
                    PlanetSurface * surf = getSurfaceFromJson(res, cache);
                    if (surf == nullptr) {
                        std::cout << "[WARNING] discarding error packet on non-existant PlanetSurface\n";
                        return;
                    }

                    surf->hud->showPopup(res["error_message"].asString());
                }

                case ERR_INVALID_CREDENTIALS: {
                    std::cout << "Invalid password\n";
                }
                case ERR_NOT_AUTHENTICATED: std::cout << "Not authenticated\n";
                case ERR_NOT_AUTHORISED:    std::cout << "Not authorised\n";
            }
            continue;
        }
        
        if (res["request"] == "getSector") {
            Sector s(res["result"]);
            cache->setSectorAt(res["x"].asInt(), res["y"].asInt(), s);
            
        } else if (res["request"] == "getSurface") {
            SurfaceLocator loc = getSurfaceLocatorFromJson(res["loc"]);
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
            if (surface == nullptr || !surface->generated) {
                std::cout << "[WARNING] discarding setTimer packet on non-existant PlanetSurface\n";
                return;
            }
            Tile * target = &surface->tiles[root["tile"].asInt()];
            surface->data->timers.push_back((Timer){target, root["time"].asDouble()});
        }
        if (root["serverRequest"].asString() == "statsChange") {
            PlanetSurface * surface = getSurfaceFromJson(root, cache);
            if (surface == nullptr || !surface->generated) {
                std::cout << "[WARNING] discarding statsChange packet on non-existant or partially loaded PlanetSurface\n";
                return;
            }
            for (auto &elem: root["resources"].getMemberNames()) {
                int key = res_json_key_to_id(elem.c_str());
                if (key < 0) {
                    //Server sent nonsense?
                    //ignore for now
                    std::cout << "[WARNING] resource key '" << elem << "' is not recognised. Ignoring\n";
                } else {
                    surface->data->stats.values[key].value    = root["resources"][elem]["value"].asDouble();
                    surface->data->stats.values[key].capacity = root["resources"][elem]["capacity"].asDouble();
                }
            }
        }
        if (root["serverRequest"].asString() == "changeTile") {
            PlanetSurface * surface = getSurfaceFromJson(root, cache);
            if (surface == nullptr || !surface->generated) {
                std::cout << "[WARNING] discarding changeTile packet on non-existant PlanetSurface\n";
                return;
            }
            surface->tiles[root["tilePos"].asInt()].type = (TileType)root["type"].asInt();
            surface->updateDirectionalTiles();
        }

        if (root["serverRequest"].asString() == "updateTileError") {
            PlanetSurface *surf = getSurfaceFromJson(root, cache);
            if (surf == nullptr || !surf->generated || surf->tiles.size() <= root["tileError"]["pos"].asUInt()) {
                std::cout << "[WARNING] discarding updateTileError packet on non-existant or partially loaded PlanetSurface\n";
                return;
            }
            surf->tiles[root["tileError"]["pos"].asUInt()].addError(root["tileError"]["msg"].asString());
        }
        
    }
}

void sendUserAction(Tile * target, TaskType task) {
    Json::Value json;
    json["request"] = "userAction";
    json["action"] = (int)task;
    json["x"] = target->x;
    json["y"] = target->y;

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

void ClientNetwork::connect(std::string address, uint16_t port, SectorCache * cache,
    std::string username, std::string password) {
    this->cache = cache;
    asio::error_code ec;

    asio::ip::tcp::resolver resolver(ctx);
    auto endpoint = resolver.resolve(address, std::to_string(port));
    asio::connect(socket.next_layer(), endpoint);
    socket.handshake(asio::ssl::stream_base::client);

    readUntil();
    std::thread asioThread = std::thread([&]() {ctx.run();});
    asioThread.detach();

    Json::Value request;
    request["request"] = "login";
    request["username"] = username;
    request["password"] = password;
    app->client.sendRequest(request);
}

void ClientNetwork::handler(std::error_code ec, size_t bytes_transferred) {
    //std::cout << bytes_transferred << "\n";
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
        //readUntil();
    }
}

void ClientNetwork::readUntil() {
    asio::async_read_until(socket, buf, '\n', [this] (std::error_code ec, std::size_t bytes_transferred) {
        handler(ec, bytes_transferred);
    });
}

