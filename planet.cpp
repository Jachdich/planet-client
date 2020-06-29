#include "planet.h"

#include "client.h"
#include "helperfunctions.h"
#include "olcPixelGameEngine.h"

Planet::Planet() {}
Planet::Planet(Json::Value res, int posInStar) {
    mass = res["mass"].asDouble();
    radius = res["radius"].asInt();
    numColours = res["numColours"].asInt();
    int col = res["baseColour"].asInt();
    baseColour = olc::Pixel(col >> 16, (col >> 8) & 0xFF, col & 0xFF);
    posFromStar = res["posFromStar"].asInt();
    theta = res["theta"].asDouble();
    angularVelocity = res["angularVelocity"].asDouble();
    
    this->generationChances = std::vector<double>(this->numColours);
    this->generationColours = std::vector<olc::Pixel>(this->numColours);
    this->generationZValues = std::vector<int>(this->numColours);
    this->generationNoise   = std::vector<double>(this->numColours);
    
    for (int i = 0; i < numColours; i++) {
        int col = res["generationColours"][i].asInt();
        generationColours[i] = olc::Pixel(col >> 16, (col >> 8) & 0xFF, col & 0xFF);
        
        generationChances[i] = res["generationChances"][i].asDouble();
        generationZValues[i] = res["generationZValues"][i].asInt();
        generationNoise[i]   = res["generationNoise"][i].asDouble();
    }
    surface = new PlanetSurface();
    this->posInStar = posInStar;
}

void Planet::draw(olc::PixelGameEngine * e, double x, double y, CamParams trx) {
    for (int ya = 0; ya < this->radius * 2; ya++) {
        for (int xa = 0; xa < this->radius * 2; xa++) {
            int xb = xa - this->radius;
            int yb = ya - this->radius;
            if ((xb * xb + yb * yb) >= (this->radius * this->radius)) continue;

            float xc = xb + x;
            float yc = yb + y;

            int r = 0;
            int g = 0;
            int b = 0;
            int total = 0;
            for (int i = 0; i < this->numColours; i++) {
                if ((noiseGen.GetNoise(xb / this->generationNoise[i], yb / this->generationNoise[i], this->generationZValues[i]) + 1) / 2 > this->generationChances[i]) {
                    r += this->generationColours[i].r;
                    g += this->generationColours[i].g;
                    b += this->generationColours[i].b;
                    total += 1;
                }
            }
            if (total == 0) {
                r = this->baseColour.r;
                g = this->baseColour.g;
                b = this->baseColour.b;
            } else {
                r /= total;
                g /= total;
                b /= total;
            }
            e->Draw(xc * trx.zoom + trx.tx, yc * trx.zoom + trx.ty, olc::Pixel(r, g, b));
        }
    }
}

void Planet::loadSurface(int secX, int secY, int starPos, int planetPos) {
	if (!surface->generated) {
		if (!surface->requested) {
		    Json::Value root;
		    root["request"] = "getSurface";
		    root["secX"] = secX;
		    root["secY"] = secY;
		    root["starPos"] = starPos;
		    root["planetPos"] = planetPos;
		    surface->requested = true;
		    std::lock_guard<std::mutex> lock(netq_mutex);
		    netRequests.push_back(root);
		    netq.notify_all();
		}
	}
}

void Planet::drawSurface(olc::PixelGameEngine * e, CamParams trx) {
    surface->draw(e, trx, this);
}
