#ifndef __GALAXY_H
#define __GALAXY_H

#include "scene.h"

class Galaxy : public Scene{

public:
    Galaxy();
    bool draw(float fElapsedTime) override;

};

#endif
