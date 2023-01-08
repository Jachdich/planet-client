#include "planet.h"

#include "client.h"
#include "helperfunctions.h"
#include "olcPixelGameEngine.h"
#include "star.h"
#include "planetdata.h"

#include <random>

Planet::Planet() {}
Planet::Planet(Json::Value res, int posInStar) {
    radius = res["radius"].asInt();
    posFromStar = res["posFromStar"].asInt();
    theta = res["theta"].asDouble();
    angularVelocity = res["angularVelocity"].asDouble();
    rotationTheta = 0;
    
    std::uniform_real_distribution<double> unif(0, 10);
    std::default_random_engine re;
    rotationalAngularVelocity = unif(re);
    this->posInStar = posInStar;
    surface = PlanetSurface(res["surface"], radius);
}

void Planet::draw(olc::PixelGameEngine * e, double x, double y, CamParams &trx) {
    if (this->surf == nullptr) {
        this->surf = new olc::Sprite(radius * 2, radius * 2);
        e->SetDrawTarget(this->surf);
        
        for (int ya = 0; ya < this->radius * 2; ya++) {
            for (int xa = 0; xa < this->radius * 2; xa++) {
                int xb = xa - this->radius;
                int yb = ya - this->radius;
                if ((xb * xb + yb * yb) > (this->radius * this->radius)) {
                    e->Draw(xa, ya, olc::Pixel(0, 0, 0, 0));
                    continue;
                }
                olc::Pixel tint = surface.tiles[ya * radius * 2 + xa].tint;
                printf("%x %x %x %x\n", tint.n, tint.r, tint.g, tint.b);
                e->Draw(xa, ya, tint);
            }
        }
        
        e->SetDrawTarget(nullptr);
        
        this->dec = new olc::Decal(this->surf);
    }
    e->DrawRotatedDecal({(float)(x * trx.zoom + trx.tx), (float)(y * trx.zoom + trx.ty)}, this->dec, this->rotationTheta, {(float)radius, (float)radius}, {trx.zoom, trx.zoom});
}

void Planet::drawSurface(olc::PixelGameEngine * e, CamParams &trx) {
    surface.draw(e, trx);
}
