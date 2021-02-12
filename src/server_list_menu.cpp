#include "server_list_menu.h"
#include "text_input_field.h"

using namespace GUI;

ServerListMenu::ServerListMenu() : Scene(){

    add_server_name = new TextInputField("add_server_name", {0, 0}, "", false, 20, 1.5);
    add_server_ip = new TextInputField("add_server_ip", {0, 100}, "", false, 20, 1.5);

    items.push_back(add_server_name);
    items.push_back(add_server_ip);

}

void ServerListMenu::addServer(const std::string& name, const std::string& ip){
    return;
}

bool ServerListMenu::draw(){
    Scene::draw();
    return true;
}