#ifndef ARTEFACT_CLASS_H
#define ARTEFACT_CLASS_H

#include "GameLevel.h"
#include "GameCube.h"
#include "Randoms.h"

class Artefact : public StaticMeshComponent {
public:
	const char* name = "";
	bool active = true;
	Artefact(const char* _name, GameLevel* level);
	void decideOnPosition(Tile* avoidanceTile, RandomGenerator* generator);
	void collect();
	void update(float deltaTime) override;
};

#endif