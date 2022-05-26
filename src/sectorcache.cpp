#include "sectorcache.h"

#include <asio.hpp>

#include "client.h"
#include "helperfunctions.h"
#include "olcPixelGameEngine.h"

using asio::ip::tcp;

SectorCache::SectorCache() {
}

void SectorCache::getSectorFromNetwork(int32_t x, int32_t y) {
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
        getSectorFromNetwork(x, y);
        secs[index].setRequested();
    }
    
    return &secs.at(index);
}

void SectorCache::draw(olc::PixelGameEngine * e, CamParams &trx) {
    int32_t sectorX = ((-trx.tx) / trx.zoom) / 256;
    int32_t sectorY = ((-trx.ty) / trx.zoom) / 256;

    if (trx.tx > 0) sectorX -= 1;
    if (trx.ty > 0) sectorY -= 1;
    

    float num = ceil((WIDTH) / 256.0 / trx.zoom);

    std::vector<olc::vi2d> starpos;
    std::vector<olc::Pixel> starcol;
    std::lock_guard<std::mutex> lock(cache_mutex);
    for (int32_t x = 0; x < num + 1; x++) {
        for (int32_t y = 0; y < num + 1; y++) {
            Sector * s = getSectorAt(sectorX + x, sectorY + y);
            if (s != nullptr && s->generated) {
                s->draw(e, trx);
                for (Star &st : s->stars) {
                    starpos.push_back(olc::vi2d{(int)((st.x + (sectorX + x) * 256) * trx.zoom + trx.tx), (int)((st.y + (sectorY + y) * 256) * trx.zoom + trx.ty)});
                    starcol.push_back(st.factionColour);
                }
            }
        }
    }
    /*
    if (starpos.size() <= 0) return;
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            double minDist = WIDTH * HEIGHT * WIDTH * HEIGHT * 0xFF;
            uint32_t minIdx = 0;
            for (uint32_t k = 0; k < starpos.size(); k++) {
                olc::vi2d delta = olc::vi2d{i, j} - starpos[k];
                double dist = abs(delta.x) + abs(delta.y);//delta.mag2();
                if (dist < minDist) {
                    minDist = dist;
                    minIdx = k;
                }
            }
            olc::Pixel col = starcol[minIdx];
            e->Draw(i, j, col);
        }
    }*/
}
