#ifndef __STARVIEW_H
#define __STARVIEW_H

#include "scene.h"

class StarView : public Scene {

public:
    StarView();
    bool draw(float fElapsedTime) override;

};

#endif
