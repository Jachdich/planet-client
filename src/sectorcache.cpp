#include "sectorcache.h"

#include <asio.hpp>

#include "client.h"
#include "helperfunctions.h"
#include "olcPixelGameEngine.h"

using asio::ip::tcp;

SectorCache::SectorCache() {
}

void SectorCache::getSectorFromNetwork(uint32_t x, uint32_t y) {
    Json::Value json;
    json["request"] = "getSector";
    json["x"] = x;
    json["y"] = y;
    app->client.sendRequest(json);
}

void SectorCache::setSectorAt(int32_t sx, int32_t sy, Sector s) {
    std::lock_guard<std::mutex> lock(cache_mutex);
    secs[((uint64_t)sx << 32) | (uint32_t)sy] = s;
}

Sector * SectorCache::getSectorAt(int32_t x, int32_t y) {
    uint64_t index = (uint64_t)x << 32 | (uint32_t)y;

    if (!secs[index].generated && !secs[index].requested) {
        getSectorFromNetwork((uint32_t)x, (uint32_t)y);
        secs[index].setRequested();
    }
    
    return &secs.at(index);
}

void SectorCache::draw(olc::PixelGameEngine * e, CamParams trx) {
    int32_t sectorX = ((-trx.tx) / trx.zoom) / 256;
    int32_t sectorY = ((-trx.ty) / trx.zoom) / 256;
    float num = ceil((WIDTH) / 256.0 / trx.zoom);
    std::lock_guard<std::mutex> lock(cache_mutex);
    for (int32_t x = 0; x < num + 1; x++) {
        for (int32_t y = 0; y < num + 1; y++) {
            Sector * s = getSectorAt(sectorX + x, sectorY + y);
            if (s != nullptr && s->generated) {
                s->draw(e, trx);
            }
        }
    }
}
