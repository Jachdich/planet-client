#ifndef __HELPERFUNCTIONS_H
#define __HELPERFUNCTIONS_H

#include <jsoncpp/json/json.h>

struct CamParams {
    int tx = 0;
    int ty = 0;
    float zoom = 1;
};

Json::Value makeJSON(std::string x);


#endif
