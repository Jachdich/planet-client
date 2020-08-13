#ifndef __HELPERFUNCTIONS_H
#define __HELPERFUNCTIONS_H

#include <jsoncpp/json/json.h>
#include "olcPixelGameEngine.h"

class CamParams {
public:
    int tx = 0;
    int ty = 0;
    float zoom = 1;
	olc::vf2d toScreen(olc::vf2d pos);
};

Json::Value makeJSON(std::string x);


#endif
