#include "helperfunctions.h"
#include "olcPixelGameEngine.h"

#include <jsoncpp/json/json.h>
#include <iostream>

Json::Value makeJSON(std::string x) {
    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();

    Json::Value root;
    std::string errors;

    bool parsingSuccessful = reader->parse(
        x.c_str(),
        x.c_str() + x.size(),
        &root,
        &errors
    );
    delete reader;
    //parsingSuccessful ??!??! std::cout << "Parsing error!\n";
    if (!parsingSuccessful) {
        return Json::Value(); //TODO error cos it didnt parse
    }
    return root;
}

olc::vf2d CamParams::toScreen(olc::vf2d pos) {
    return (pos * zoom) + olc::vf2d(tx, ty);
}

std::string toHexString(std::string initText, olc::Pixel colour) {
    std::stringstream stream;
    stream << initText << std::setfill('0') << std::setw(6) << std::hex << ((((colour.r << 8) | colour.g) << 8) | colour.b);
    return stream.str();
}

//.kick (what if they're the last player?)
//.leave (what if you're the last player?)
//wining
//leaderboard
//need to say uno
//stack??!
