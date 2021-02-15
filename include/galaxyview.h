#ifndef __GALAXY_H
#define __GALAXY_H

#include "scene.h"

class Galaxy : public GUI::Scene{

public:
    Galaxy();
    bool draw() override;

};

#endif