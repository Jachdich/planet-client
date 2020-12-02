#include "sectorcache.h"

#include <asio.hpp>

//PLEASE DELETE TODO
#include <iostream>
#include "client.h"
#include "helperfunctions.h"
#include "olcPixelGameEngine.h"

using asio::ip::tcp;

SectorCache::SectorCache(tcp::socket * sock) {
    this->sock = sock;
}

void SectorCache::getSectorFromNetwork(int x, int y) {
    //std::cout << "Requesting sector " << x << " " << y << "\n";
    Json::Value json;
    json["request"] = "getSector";
    json["x"] = x;
    json["y"] = y;
    std::lock_guard<std::mutex> lock(netq_mutex);
    netRequests.push_back(json);
    netq.notify_all();
}

void SectorCache::setSectorAt(int sx, int sy, Sector s) {
    std::lock_guard<std::mutex> lock(cache_mutex);
    secs[(long long)sx << 32 | sy] = s;
}

Sector * SectorCache::getSectorAt(int x, int y) {
    long long index = (long long)x << 32 | y;
    if (x < 0 || y < 0) {
        index = 0;
    }
    
    if (!secs[index].generated && !secs[index].requested) {
        getSectorFromNetwork(x, y);
        secs[index].setRequested();
    }
    
    return &secs.at(index);
}

void SectorCache::draw(olc::PixelGameEngine * e, CamParams trx) {
    int sectorX = ((-trx.tx) / trx.zoom) / 256;
    int sectorY = ((-trx.ty) / trx.zoom) / 256;
    float num = ceil((WIDTH) / 256.0 / trx.zoom);
    std::lock_guard<std::mutex> lock(cache_mutex);
    for (int x = 0; x < num + 1; x++) {
        for (int y = 0; y < num + 1; y++) {
            Sector * s = getSectorAt(sectorX + x, sectorY + y);
            if (s != nullptr && s->generated) {
                s->draw(e, trx);
            }
        }
    }
}
