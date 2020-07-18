#include "network.h"

#include <iostream>
#include "client.h"
#include "planet.h"
#include "planetsurface.h"
#include "sector.h"
#include "star.h"

void handleNetwork(tcp::socket * sock, SectorCache * cache) {
	//return;
    while (true) {
        std::unique_lock<std::mutex> lk(netq_mutex);
        netq.wait(lk);
        lk.unlock();
        if (netThreadStop) { return; }
        Json::Value totalJSON;
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
            }
        }
    }
}
