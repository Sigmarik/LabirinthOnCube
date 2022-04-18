#include "GameCube.h"

Cube::Cube(int size, RandomGenerator& generator, GameLevel* level) {
	side = size;
	parentLevel = level;
	std::cout << "GenT ";
	top    = GameSide(side, generator, level, 0);
	std::cout << "GenB ";
	bottom = GameSide(side, generator, level, side * side);
	std::cout << "GenF ";
	front  = GameSide(side, generator, level, side * side * 2);
	std::cout << "GenB ";
	back   = GameSide(side, generator, level, side * side * 3);
	std::cout << "GenL ";
	left   = GameSide(side, generator, level, side * side * 4);
	std::cout << "GenR ";
	right  = GameSide(side, generator, level, side * side * 5);
	std::cout << "Gen\n";
	//attach(&top);
	//attach(&bottom);
	//attach(&left);
	//attach(&right);
	//attach(&front);
	//attach(&back);
	top.transform    = glm::translate(glm::mat4(1.0), glm::vec3(0.0, (float)size / 2.0, 0.0));
	bottom.transform = glm::translate(glm::mat4(1.0), glm::vec3(0.0, -(float)size / 2.0, 0.0));
	front.transform  = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, -(float)size / 2.0));
	back.transform   = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, (float)size / 2.0));
	left.transform   = glm::translate(glm::mat4(1.0), glm::vec3((float)size / 2.0, 0.0, 0.0));
	right.transform  = glm::translate(glm::mat4(1.0), glm::vec3(-(float)size / 2.0, 0.0, 0.0));
	
	bottom.transform = glm::rotate(bottom.transform, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));
	front.transform  = glm::rotate(front.transform,  glm::radians(90.0f),  glm::vec3(-1.0, 0.0, 0.0));
	back.transform   = glm::rotate(back.transform,   glm::radians(-90.0f), glm::vec3(-1.0, 0.0, 0.0));
	left.transform   = glm::rotate(left.transform,   glm::radians(-90.0f), glm::vec3(0.0, 0.0, 1.0));
	right.transform  = glm::rotate(right.transform,  glm::radians(90.0f),  glm::vec3(0.0, 0.0, 1.0));
	children.push_back(&bottom);
	children.push_back(&top);
	children.push_back(&left);
	children.push_back(&right);
	children.push_back(&front);
	children.push_back(&back);
}

Tile* Cube::checkPixel(Camera* camera, glm::vec2 pixel) {
	Tile* answer = nullptr;
	for (GameSide* side : sides()) {
		Tile* hit = side->checkPixel(camera, pixel);
		if (hit && (!answer || glm::distance(glm::vec3(hit->worldMatrix()[3]), camera->position) < 
			glm::distance(glm::vec3(answer->worldMatrix()[3]), camera->position))) {
			answer = hit;
		}
	}
	return answer;
}

std::vector<GameSide*> Cube::sides() {
	return { &top, &bottom, &left, &right, &front, &back };
}

void Cube::resetHighlights(bool resetFull) {
	for (GameSide* gSide : sides()) {
		for (int i = 0; i < side; i++) {
			for (int j = 0; j < side; j++) {
				if (resetFull || gSide->tiles[i][j]->highlight < 0.99) gSide->tiles[i][j]->highlight = 0.0;
			}
		}
	}
}

void Cube::receveInput(int key, bool flushSelection) {
	// Do nothing for now.
}