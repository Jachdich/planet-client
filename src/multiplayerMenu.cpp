#include "multiplayerMenu.h"
#include "button.h"
#include "client.h"

MultiplayerMenu::MultiplayerMenu() : Scene("textures/backgrounds/background_multiplayer.png"){
    Button* item =  new Button("server_list", {0, 430}, "", "Server List", false, 1.5, 16);
    Button* item2 =  new Button("connect_to_ip", {0, 460}, "", "Connect To IP", false, 1.5, 16);
    item2->setOnClick([](){app->changeScene(app->connectToIPMenu.get());});
    Button* item3 =  new Button("back_to_menu", {0, 490}, "", "Back To Menu", false, 1.5, 16);
    item3->setOnClick([](){app->changeScene(app->mainMenu.get());});
    items.push_back(item);
    items.push_back(item2);
    items.push_back(item3);
}
