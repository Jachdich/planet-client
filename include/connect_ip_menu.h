#ifndef __CONNECT_IP_MENU_H
#define __CONNECT_IP_MENU_H

#include "scene.h"
#include "text_input_field.h"

class ConnectToIpMenu : public Scene{

private:
    TextInputField* ip = new TextInputField("ip", {0, 400}, "", true, 10, 2);

public:
    ConnectToIpMenu();
    bool draw() override;
    void onGUIItemClicked(const std::string& name) override;
};

#endif