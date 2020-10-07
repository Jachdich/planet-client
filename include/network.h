#ifndef __NETWORK_H
#define __NETWORK_H

#include <asio.hpp>

#include "sectorcache.h"
#include "common/enums.h"
#include "tile.h"
using asio::ip::tcp;

void handleNetwork(tcp::socket * sock, SectorCache * cache);
void sendChangeTileRequest(Tile * target, TileType to);
void sendUserAction(Tile * target, TaskType task, std::function<void(int, ErrorCode)> callback);
#endif
