#ifndef __NETWORK_H
#define __NETWORK_H

#include <asio.hpp>

#include "sectorcache.h"
#include "common/enums.h"
#include "tile.h"

using asio::ip::tcp;

class ClientNetwork {
public:
    std::vector<char> buf;
    asio::io_context ctx;
    asio::ip::tcp::socket socket;
    SectorCache * cache;
    
    void readUntil();
    void handler(std::error_code ec, size_t bytes_transferred);

    ClientNetwork();
    void connect(std::string address, uint16_t port, SectorCache * cache);
    
};

//void handleNetwork(tcp::socket * sock, SectorCache * cache);
void sendChangeTileRequest(Tile * target, TileType to);
void sendUserAction(Tile * target, TaskType task, std::function<void(int, ErrorCode)> callback);
#endif
