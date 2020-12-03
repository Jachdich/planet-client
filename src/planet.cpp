#include "planet.h"

#include "client.h"
#include "helperfunctions.h"
#include "olcPixelGameEngine.h"
#include "star.h"

#include <random>

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
	
	double lower_bound = 0;
    double upper_bound = 10;
    std::uniform_real_distribution<double> unif(lower_bound,upper_bound);
    std::default_random_engine re;
    rotationalAngularVelocity = unif(re);

    
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

std::string toHexString(std::string initText, olc::Pixel colour) {
	std::stringstream stream;
	stream << initText << std::setfill('0') << std::setw(6) << std::hex << ((((colour.r << 8) | colour.g) << 8) | colour.b);
	return stream.str();
}

void Planet::draw(olc::PixelGameEngine * e, double x, double y, CamParams trx) {
	if (this->surf == nullptr) {
		this->surf = new olc::Sprite(radius * 2, radius * 2);
		e->SetDrawTarget(this->surf);
		
		for (int ya = 0; ya < this->radius * 2; ya++) {
			for (int xa = 0; xa < this->radius * 2; xa++) {
				int xb = xa - this->radius;
				int yb = ya - this->radius;
				if ((xb * xb + yb * yb) >= (this->radius * this->radius)) {
					e->Draw(xa, ya, olc::Pixel(0, 0, 0, 0));
					continue;
				}
				
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
				e->Draw(xa, ya, olc::Pixel(r, g, b));
			}
		}
		
		e->SetDrawTarget(nullptr);
		
		this->dec = new olc::Decal(this->surf);
	}
	e->DrawRotatedDecal({(float)(x * trx.zoom + trx.tx), (float)(y * trx.zoom + trx.ty)}, this->dec, this->rotationTheta, {(float)radius, (float)radius}, {trx.zoom, trx.zoom}); 
	int sx = (x + radius * 2) * trx.zoom + trx.tx;
	int sy = (y - radius * 2) * trx.zoom + trx.ty;
	if (debugMode) {
		e->DrawStringDecal({(float)sx, (float)sy}, toHexString("baseColour: #", baseColour), baseColour);
		e->DrawStringDecal({(float)sx, (float)(sy += 10)}, "numColours: " + std::to_string(numColours), olc::Pixel(255, 255, 255));
		for (int i = 0; i < numColours; i++) {
			e->DrawStringDecal({(float)sx, (float)(sy += 10)}, "Colour " + std::to_string(i) + ": " + toHexString("#", generationColours[i])
			+ " Noise: " + std::to_string(generationNoise[i])
			+ " ZVal: " + std::to_string(generationZValues[i])
			+ " Chance: " + std::to_string(generationChances[i])
			, generationColours[i]);
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
		    app->client.sendRequest(root);
		}
	}
}

void Planet::drawSurface(olc::PixelGameEngine * e, CamParams trx) {
    surface->draw(e, trx);
}
