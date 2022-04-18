#ifndef SAR_DOME_CLASS_H
#define SAR_DOME_CLASS_H

#include "StaringComponent.h"
#include "Randoms.h"

class StarDome : public GameComponent {
public:
	StarDome(GameLevel* level, int count, RandomGenerator* generator);
};

#endif