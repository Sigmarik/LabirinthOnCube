#include "Artefact.h"

//template std::vector<Artefact*> GameComponent::getAllChildrenOfClass<Artefact*>();

Artefact::Artefact(const char* _name, GameLevel* level) {
	name = _name;
	mesh = level->getMesh(name);
	parentLevel = level;
}

void Artefact::decideOnPosition(Tile* avoidanceTile, RandomGenerator* generator) {
	Cube* cube = (Cube*)avoidanceTile->parent->parent;
	std::vector<GameSide*> priorityList(6, nullptr);
	if (avoidanceTile->parent == &cube->top)
		priorityList = { &cube->bottom, &cube->left, &cube->right, &cube->front, &cube->back, &cube->top };
	if (avoidanceTile->parent == &cube->bottom)
		priorityList = { &cube->top, &cube->left, &cube->right, &cube->front, &cube->back, &cube->bottom };
	if (avoidanceTile->parent == &cube->left)
		priorityList = { &cube->right, &cube->top, &cube->bottom, &cube->front, &cube->back, &cube->left };
	if (avoidanceTile->parent == &cube->right)
		priorityList = { &cube->left, &cube->top, &cube->bottom, &cube->front, &cube->back, &cube->right };
	if (avoidanceTile->parent == &cube->front)
		priorityList = { &cube->back, &cube->right, &cube->top, &cube->bottom, &cube->left, &cube->front };
	if (avoidanceTile->parent == &cube->back)
		priorityList = { &cube->front, &cube->right, &cube->top, &cube->bottom, &cube->left, &cube->back };
	GameSide* chosenSide = nullptr;
	int cellI = 0, cellJ = 0;
	do {
		int decision = generator->range(0, 100);
		int primaryChance = 60;
		int secondaryChance = 10;
		if (decision < primaryChance)
			chosenSide = priorityList[0];
		else if (primaryChance <= decision && decision < 100 - secondaryChance)
			chosenSide = priorityList[1 + (decision - primaryChance) / ((100 - secondaryChance - primaryChance) / 4)];
		else
			chosenSide = priorityList[5];
		cellI = generator->range(0, chosenSide->size);
		cellJ = generator->range(0, chosenSide->size);
	} while (chosenSide->tiles[cellI][cellJ]->openedRouts == 0 || 
		chosenSide->tiles[cellI][cellJ]->hasArtefact);
	std::cout << "Chosen position -> " << cellI << ' ' << cellJ << " of game side " << chosenSide << std::endl;
	chosenSide->tiles[cellI][cellJ]->hasArtefact = true;
	parent = chosenSide->tiles[cellI][cellJ];
	parent->children.push_back(this);
}

void Artefact::collect() {
	active = false;
}

void Artefact::update(float deltaTime) {
	if (!active) {
		float multiplyer = glm::max(0.0, 1.0 - 5.0 * deltaTime);
		transform[0] *= multiplyer;
		transform[1] *= multiplyer;
		transform[2] *= multiplyer;
	}
}