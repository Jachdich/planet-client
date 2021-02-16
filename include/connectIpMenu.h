#ifndef __CONNECT_IP_MENU_H
#define __CONNECT_IP_MENU_H

#include "scene.h"
#include "textInputField.h"

class ConnectToIpMenu : public Scene{

private:
    TextInputField* ip;

public:
    ConnectToIpMenu();
    bool draw() override;
    void onEnterPressed(const std::string& name, const std::string& text);
};

#endif