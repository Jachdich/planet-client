#ifndef __STARVIEW_H
#define __STARVIEW_H

#include "scene.h"

class StarView : public GUI::Scene{

public:
    StarView();
    bool draw() override;

};

#endif