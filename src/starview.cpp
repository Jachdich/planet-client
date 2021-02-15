#include "starview.h"
#include "client.h"
#include "planetdata.h"

StarView::StarView() : Scene(""){

}

bool StarView::draw(){
    Scene::draw();

    if(app->GetMouse(0).bPressed){
        Planet * p = app->selectedStar->getPlanetAt(app->GetMouseX(), app->GetMouseY(), app->trx);
        if (p != nullptr) {
            app->selectedPlanet = p;
            if (!app->selectedPlanet->surface->generated and !app->selectedPlanet->surface->requested) {
        		app->selectedPlanet->loadSurface(app->lastClickedSector->x, app->lastClickedSector->y, app->selectedStar->posInSector, app->selectedPlanet->posInStar);
        	}
            app->trx = {0, 0, 1};
        }
    }
    
    app->selectedStar->drawWithPlanets(app, fElapsedTime, app->trx);

    return true;
}