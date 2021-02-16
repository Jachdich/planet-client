#ifndef __PLANETVIEW_H
#define __PLANETVIEW_H

#include "scene.h"

class PlanetView : public Scene{

public:
    PlanetView();
    bool draw() override;

};

#endif