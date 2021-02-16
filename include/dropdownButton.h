#ifndef __DROPDOWNBUTTON_H
#define __DROPDOWNBUTTON_H

#include "button.h"

class DropdownButton : public GUI::Button{

public:
    DropdownButton();

    bool draw() override;

};

#endif