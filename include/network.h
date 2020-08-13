#ifndef __NETWORK_H
#define __NETWORK_H

#include <asio.hpp>

#include "sectorcache.h"
using asio::ip::tcp;

void handleNetwork(tcp::socket * sock, SectorCache * cache);
#endif
