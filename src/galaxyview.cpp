#include "galaxyview.h"
#include "client.h"

Galaxy::Galaxy() : Scene(""){
}

bool Galaxy::draw(float fElapsedTime){
    Scene::draw();

    if(app->GetMouse(0).bPressed){
        Sector * s = app->map.getSectorAt(floor((app->GetMouseX() - app->trx.tx) / app->trx.zoom / 256),
                                        floor((app->GetMouseY() - app->trx.ty) / app->trx.zoom / 256));
            app->lastClickedSector = s;
 
            Star * st = s->getStarAt(app->GetMouseX(), app->GetMouseY(), app->trx);

            if (st != nullptr) {
                app->selectedStar = st;
                app->galaxyTrx = app->trx;
                app->trx = {0, 0, 1};
                app->changeScene(app->starViewScene.get());
            }
    }
    app->map.draw(app, app->trx);

    return true;
}
