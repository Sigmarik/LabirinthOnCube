#ifndef CHECKER_SIDE_CLASS_H
#define CHECKER_SIDE_CLASS_H

#define FIELD_SHIFT_UP 0
#define FIELD_SHIFT_DOWN 1
#define FIELD_SHIFT_LEFT 2
#define FIELD_SHIFT_RIGHT 3

#include <algorithm>

#include "Tile.h"

#include "tform.h"
#include "GameLevel.h"

class GameSide : public GameComponent {
public:
	int size = 3;
	std::vector<std::vector<Tile*>> tiles;
	GameLevel* parentLevel = nullptr;
	GameSide(GameLevel* level = nullptr);
	GameSide(int side, RandomGenerator& generator, GameLevel* level, int startIndex = 0);
	void assignHintSquares();
	void reassignChildren();
	Tile* shift(Tile* insertion, int x, int y, int direction, float time);
	Tile* checkPixel(Camera* camera, glm::vec2 pixel);
	//Tile* rayHit(glm::vec3 start, glm::vec3 direction);
};

#endif