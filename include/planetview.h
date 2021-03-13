#ifndef __PLANETVIEW_H
#define __PLANETVIEW_H

#include "scene.h"
#include "dropdownButton.h"

class PlanetView : public Scene{

public:
    DropdownButton* curr_ddb;
    PlanetView();
    bool draw() override;

};

#endif