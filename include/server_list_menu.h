#ifndef __SERVER_LIST_MENU_H
#define __SERVER_LIST_MENU_H

#include "scene.h"
#include "text_input_field.h"

class ServerListMenu : public GUI::Scene{

private:
    GUI::TextInputField* add_server_name;
    GUI::TextInputField* add_server_ip;
    void loadServers();
    void saveServers();
    int server_count;
    int max_servers = 3;

public:
    ServerListMenu();
    void addServer(const std::string& name, const std::string& ip);
    bool draw() override;

};

#endif