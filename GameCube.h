#ifndef CUBE_CLASS_H
#define CUBE_CLASS_H

#include "GameSide.h"

#include "GameLevel.h"

class Cube : public GameComponent {
public:
	GameSide top, bottom, front, back, left, right;
	int side;
	GameLevel* parentLevel = nullptr;
	Tile* activeTile = nullptr;
	Cube(int size, RandomGenerator& generator, GameLevel* level);
	Tile* checkPixel(Camera* camera, glm::vec2 pixel);
	std::vector<GameSide*> sides();
	void resetHighlights(bool resetFull = false);
	void receveInput(int key, bool flushSelection = true);
};

#endif