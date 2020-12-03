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
    void getSectorFromNetwork(int x, int y);
    void setSectorAt(int sx, int sy, Sector s);
    Sector * getSectorAt(int x, int y);
    void draw(olc::PixelGameEngine * e, CamParams trx);
};
#endif
