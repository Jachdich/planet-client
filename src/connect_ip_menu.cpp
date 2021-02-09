#include "connect_ip_menu.h"
#include "text_input_field.h"
#include "client.h"
#include "main_menu.h"

ConnectToIpMenu::ConnectToIpMenu() : Scene(){

    Button* item =  new Button("back_to_menu", {0, 490}, "", "Back To Menu", false, 1.5, 16);
    items.push_back(item);
    items.push_back(ip);

}

bool ConnectToIpMenu::draw(){
    Scene::draw();
    if(ip->getEnterPressed()){
        app->address = ip->getText();
        app->connectToServer();
    }
    return true;
}

void ConnectToIpMenu::onGUIItemClicked(const std::string& name){

    if(name == "back_to_menu"){
        app->changeScene(app->mainMenu.get());
    }

}