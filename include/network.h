#ifndef __NETWORK_H
#define __NETWORK_H

#include <asio.hpp>
#include <asio/ssl.hpp>

#include "sectorcache.h"
#include "common/enums.h"
#include "tile.h"

using asio::ip::tcp;

class ClientNetwork {
public:
    asio::streambuf buf;
    asio::io_context ctx;
    asio::ssl::context ssl_ctx;
    asio::ssl::stream<asio::ip::tcp::socket> socket;
    SectorCache * cache;
    
    void readUntil();
    void handler(std::error_code ec, size_t bytes_transferred);
    void sendRequest(Json::Value request);

    ClientNetwork();
    void connect(std::string address, uint16_t port, SectorCache * cache);
    
};

void sendUserAction(Tile * target, TaskType task);
#endif
