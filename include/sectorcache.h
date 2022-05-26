#ifndef __SECTORCACHE_H
#define __SECTORCACHE_H

#include <asio.hpp>
#include <unordered_map>
#include "helperfunctions.h"
#include "olcPixelGameEngine.h"
#include "sector.h"

using asio::ip::tcp;

class SectorCache {
public:
    int secNum = 6;
    std::unordered_map<uint64_t, Sector> secs;
    
    SectorCache();
    void getSectorFromNetwork(int32_t x, int32_t y);
    void setSectorAt(int32_t sx, int32_t sy, Sector s);
    Sector * getSectorAt(int32_t x, int32_t y);
    void draw(olc::PixelGameEngine * e, CamParams &trx);
};
#endif
