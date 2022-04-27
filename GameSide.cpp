#include "GameSide.h"

GameSide::GameSide(GameLevel* level) {
	parentLevel = level;
};

GameSide::GameSide(int side, RandomGenerator& generator, GameLevel* level, int startIndex) {
	parentLevel = level;
	size = side;
	tiles = std::vector<std::vector<Tile*>>(size, std::vector<Tile*>(size, nullptr));
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			int routs = 0;
			for (int pointer = 0; pointer < 4; pointer++) {
				int gen = generator.range(0, 100);
				if (gen < 60) routs += (1 << pointer);
			}
			tiles[i][j] = new Tile(routs, parentLevel, i * size + j + startIndex);
			tiles[i][j]->parent = this;
			tiles[i][j]->transform = glm::translate(glm::vec3(
				(float)(i * 2 - size + 1) / 2.0, 0.0, 
				(float)(j * 2 - size + 1) / 2.0));
			tiles[i][j]->generateGeometry(generator);
			children.push_back(tiles[i][j]);
		}
	}
}

void GameSide::assignHintSquares() {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			tiles[i][j]->parentLevel = parentLevel;
		}
	}
}

Tile* GameSide::shift(Tile* insertion, int x, int y, int direction, float time) {
	Tile* answer = nullptr;
	if (direction == FIELD_SHIFT_UP) {
		answer = tiles[x][size - 1];
	}
	if (direction == FIELD_SHIFT_DOWN) {
		answer = tiles[x][0];
	}
	if (direction == FIELD_SHIFT_LEFT) {
		answer = tiles[0][y];
	}
	if (direction == FIELD_SHIFT_RIGHT) {
		answer = tiles[size - 1][y];
	}
	if (direction == FIELD_SHIFT_UP) {
		for (int t = 0; t < size - 1; t++) {
			tiles[x][size - t - 2]->updateTime = time;
			std::swap(tiles[x][size - t - 1], tiles[x][size - t - 2]);
		}
		tiles[x][0] = insertion;
	}
	if (direction == FIELD_SHIFT_DOWN) {
		for (int t = 0; t < size - 1; t++) {
			tiles[x][t + 1]->updateTime = time;
			std::swap(tiles[x][t + 1], tiles[x][t]);
		}
		tiles[x][size - 1] = insertion;
	}
	if (direction == FIELD_SHIFT_LEFT) {
		for (int t = 0; t < size - 1; t++) {
			tiles[t + 1][y]->updateTime = time;
			std::swap(tiles[t + 1][y], tiles[t][y]);
		}
		tiles[size - 1][y] = insertion;
	}
	if (direction == FIELD_SHIFT_RIGHT) {
		for (int t = 0; t < size - 1; t++) {
			tiles[size - t - 2][y]->updateTime = time;
			std::swap(tiles[size - t - 2][y], tiles[size - t - 1][y]);
		}
		tiles[0][y] = insertion;
	}
	return answer;
}

Tile* GameSide::checkPixel(Camera* camera, glm::vec2 pixel) {
	Tile* answer = nullptr;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			Tile* hit = tiles[i][j]->checkPixel(camera, pixel);
			if (hit && (!answer || glm::distance(glm::vec3(hit->worldMatrix()[3]), camera->position) <
				glm::distance(glm::vec3(answer->worldMatrix()[3]), camera->position))) {
				answer = hit;
			}
		}
	}
	return answer;
}

void GameSide::reassignChildren() {
	children.clear();
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			children.push_back(tiles[i][j]);
			tiles[i][j]->transform[3] = glm::vec4(
				(float)((i * 2 - size + 1) / 2.0), 0.0,
				(float)((j * 2 - size + 1) / 2.0), 1.0);
			tiles[i][j]->parent = this;
		}
	}
}