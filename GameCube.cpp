#include "GameCube.h"

Cube::Cube(int size, RandomGenerator& generator, GameLevel* level) {
	side = size;
	parentLevel = level;
	top    = GameSide(side, generator, level, 0);
	bottom = GameSide(side, generator, level, side * side);
	front  = GameSide(side, generator, level, side * side * 2);
	back   = GameSide(side, generator, level, side * side * 3);
	left   = GameSide(side, generator, level, side * side * 4);
	right  = GameSide(side, generator, level, side * side * 5);
	top.transform    = glm::translate(glm::mat4(1.0), glm::vec3( 0.0,  1.0,  0.0 ) * (float)size / 2.0f);
	bottom.transform = glm::translate(glm::mat4(1.0), glm::vec3( 0.0, -1.0,  0.0 ) * (float)size / 2.0f);
	front.transform  = glm::translate(glm::mat4(1.0), glm::vec3(-1.0,  0.0,  0.0 ) * (float)size / 2.0f);
	back.transform   = glm::translate(glm::mat4(1.0), glm::vec3( 1.0,  0.0,  0.0 ) * (float)size / 2.0f);
	left.transform   = glm::translate(glm::mat4(1.0), glm::vec3( 0.0,  0.0, -1.0 ) * (float)size / 2.0f);
	right.transform  = glm::translate(glm::mat4(1.0), glm::vec3( 0.0,  0.0,  1.0 ) * (float)size / 2.0f);
	
	front.transform  = glm::rotate(front.transform,  glm::radians(90.0f),  glm::vec3(0.0, 0.0, 1.0));
	back.transform   = glm::rotate(back.transform,   glm::radians(-90.0f), glm::vec3(0.0, 0.0, 1.0));
	bottom.transform = glm::rotate(bottom.transform, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));
	left.transform   = glm::rotate(left.transform,   glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	right.transform  = glm::rotate(right.transform,  glm::radians(90.0f),  glm::vec3(1.0, 0.0, 0.0));
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
	if (!activeTile) return;
	glm::vec2 localShift = glm::vec2(0.0f);
	if (key == GLFW_KEY_W) localShift = glm::vec2(-1.0,  0.0 );
	if (key == GLFW_KEY_S) localShift = glm::vec2( 1.0,  0.0 );
	if (key == GLFW_KEY_A) localShift = glm::vec2( 0.0,  1.0 );
	if (key == GLFW_KEY_D) localShift = glm::vec2( 0.0, -1.0 );
	GameSide* gSide = (GameSide*)(activeTile->parent);
	int i_it = 0, j_it = 0;
	for (int i = 0; i < side; i++) {
		for (int j = 0; j < side; j++) {
			if (gSide->tiles[i][j] != activeTile) continue;
			i_it = i;
			j_it = j;
		}
	}
	shiftCell(gSide, i_it, j_it, localShift);
	if (flushSelection) {
		activeTile->highlight = 0.0;
		activeTile = nullptr;
	}
}

void Cube::shiftCell(GameSide* gSide, int i_it, int j_it, glm::vec2 shift, int slideCount) {
	if (gSide == nullptr) return;
	if (slideCount >= side * 4 - 1) return;
	int cellI = i_it + (int)shift.x;
	int cellJ = j_it + (int)shift.y;
	glm::mat3 additionalRotation = glm::mat3(1.0);
	GameSide* targetSide = gSide;
	glm::vec2 newShift = shift;
	if (gSide == &top) {
		if (cellI < 0) {
			targetSide = &front;
			cellI += side;
		}
		if (cellI >= side) {
			targetSide = &back;
			cellI -= side;
		}
		if (cellJ < 0) {
			targetSide = &left;
			cellJ += side;
		}
		if (cellJ >= side) {
			targetSide = &right;
			cellJ -= side;

		}
	}
	if (gSide == &front) {
		if (cellI < 0) {
			targetSide = &bottom;
			cellI += side;
		}
		if (cellI >= side) {
			targetSide = &top;
			cellI -= side;
		}
		if (cellJ < 0) {
			targetSide = &left;
			cellJ = cellI;
			cellI = 0;
			additionalRotation = glm::mat3(
				glm::vec3(0.0, 0.0, 1.0),
				glm::vec3(0.0, 1.0, 0.0),
				glm::vec3(-1.0, 0.0, 0.0));
			newShift = glm::vec2(1.0, 0.0);
		}
		if (cellJ >= side) {
			targetSide = &right;
			cellJ = side - cellI - 1;
			cellI = 0;
			additionalRotation = glm::mat3(
				glm::vec3(0.0, 0.0, -1.0),
				glm::vec3(0.0, 1.0, 0.0),
				glm::vec3(1.0, 0.0, 0.0));
			newShift = glm::vec2(1.0, 0.0);
		}
	}
	if (gSide == &bottom) {
		if (cellI < 0) {
			cellI += side;
			targetSide = &back;
		}
		if (cellI >= side) {
			cellI -= side;
			targetSide = &front;
		}
		if (cellJ < 0) {
			targetSide = &left;
			cellI = side - cellI - 1;
			cellJ = 0;
			additionalRotation = glm::mat3(
				glm::vec3(-1.0, 0.0, 0.0),
				glm::vec3(0.0, 1.0, 0.0),
				glm::vec3(0.0, 0.0, -1.0));
			newShift = glm::vec2(0.0, 1.0);
		}
		if (cellJ >= side) {
			targetSide = &right;
			cellI = side - cellI - 1;
			cellJ = side - 1;
			additionalRotation = glm::mat3(
				glm::vec3(-1.0, 0.0, 0.0),
				glm::vec3(0.0, 1.0, 0.0),
				glm::vec3(0.0, 0.0, -1.0));
			newShift = glm::vec2(0.0, -1.0);
		}
	}
	if (gSide == &back) {
		if (cellI < 0) {
			cellI += side;
			targetSide = &top;
		}
		if (cellI >= side) {
			cellI -= side;
			targetSide = &bottom;
		}
		if (cellJ < 0) {
			targetSide = &left;
			cellJ = side - cellI - 1;
			cellI = side - 1;
			additionalRotation = glm::mat3(
				glm::vec3(0.0, 0.0, -1.0),
				glm::vec3(0.0, 1.0, 0.0),
				glm::vec3(1.0, 0.0, 0.0));
			newShift = glm::vec2(-1.0, 0.0);
		}
		if (cellJ >= side) {
			targetSide = &right;
			cellJ = cellI;
			cellI = side - 1;
			additionalRotation = glm::mat3(
				glm::vec3(0.0, 0.0, 1.0),
				glm::vec3(0.0, 1.0, 0.0),
				glm::vec3(-1.0, 0.0, 0.0));
			newShift = glm::vec2(-1.0, 0.0);
		}
	}
	if (gSide == &left) {
		if (cellI < 0) {
			targetSide = &front;
			cellI = cellJ;
			cellJ = 0;
			additionalRotation = glm::mat3(
				glm::vec3(0.0, 0.0, -1.0),
				glm::vec3(0.0, 1.0, 0.0),
				glm::vec3(1.0, 0.0, 0.0));
			newShift = glm::vec2(0.0, 1.0);
		}
		if (cellI >= side) {
			targetSide = &back;
			cellI = side - cellJ - 1;
			cellJ = 0;
			additionalRotation = glm::mat3(
				glm::vec3(0.0, 0.0, 1.0),
				glm::vec3(0.0, 1.0, 0.0),
				glm::vec3(-1.0, 0.0, 0.0));
			newShift = glm::vec2(0.0, 1.0);
		}
		if (cellJ < 0) {
			targetSide = &bottom;
			cellI = side - cellI - 1;
			cellJ = 0;
			additionalRotation = glm::mat3(
				glm::vec3(-1.0, 0.0, 0.0),
				glm::vec3(0.0, 1.0, 0.0),
				glm::vec3(0.0, 0.0, -1.0));
			newShift = glm::vec2(0.0, 1.0);
		}
		if (cellJ >= side) {
			targetSide = &top;
			cellJ = 0;
		}
	}
	if (gSide == &right) {
		if (cellI < 0) {
			targetSide = &front;
			cellI = side - cellJ - 1;
			cellJ = side - 1;
			additionalRotation = glm::mat3(
				glm::vec3(0.0, 0.0, 1.0),
				glm::vec3(0.0, 1.0, 0.0),
				glm::vec3(-1.0, 0.0, 0.0));
			newShift = glm::vec2(0.0, -1.0);
		}
		if (cellI >= side) {
			targetSide = &back;
			cellI = cellJ;
			cellJ = side - 1;
			additionalRotation = glm::mat3(
				glm::vec3(0.0, 0.0, -1.0),
				glm::vec3(0.0, 1.0, 0.0),
				glm::vec3(1.0, 0.0, 0.0));
			newShift = glm::vec2(0.0, -1.0);
		}
		if (cellJ < 0) {
			targetSide = &top;
			cellJ = side - 1;
		}
		if (cellJ >= side) {
			targetSide = &bottom;
			cellI = side - cellI - 1;
			cellJ = side - 1;
			additionalRotation = glm::mat3(
				glm::vec3(-1.0, 0.0, 0.0),
				glm::vec3(0.0, 1.0, 0.0),
				glm::vec3(0.0, 0.0, -1.0));
			newShift = glm::vec2(0.0, -1.0);
		}
	}
	if (!(0 <= cellI && cellI < side &&
		0 <= cellJ && cellJ < side)) {
		std::cout << "WRONG INDICES " << cellI << ' ' << cellJ << " SIDE " << side << "\n";
		return;
	}
	glm::mat3 centeredMatrix = additionalRotation * glm::mat3(gSide->tiles[i_it][j_it]->transform);
	gSide->tiles[i_it][j_it]->transform[0] = glm::vec4(centeredMatrix[0], 0.0);
	gSide->tiles[i_it][j_it]->transform[1] = glm::vec4(centeredMatrix[1], 0.0);
	gSide->tiles[i_it][j_it]->transform[2] = glm::vec4(centeredMatrix[2], 0.0);
	// Need to add inverse additional transform to targetSide tile.
	glm::mat3 inverseCentered = glm::inverse(additionalRotation) * glm::mat3(targetSide->tiles[cellI][cellJ]->transform);
	targetSide->tiles[cellI][cellJ]->transform[0] = glm::vec4(inverseCentered[0], 0.0);
	targetSide->tiles[cellI][cellJ]->transform[1] = glm::vec4(inverseCentered[1], 0.0);
	targetSide->tiles[cellI][cellJ]->transform[2] = glm::vec4(inverseCentered[2], 0.0);
	Tile* memory = targetSide->tiles[cellI][cellJ];
	targetSide->tiles[cellI][cellJ] = gSide->tiles[i_it][j_it];
	gSide->tiles[i_it][j_it] = memory;
	targetSide->reassignChildren();
	gSide->reassignChildren();
	shiftCell(targetSide, cellI, cellJ, newShift, slideCount + 1);
}

float distManhattan(glm::vec3 v_a, glm::vec3 v_b) {
	return abs(v_a.x - v_b.x) + abs(v_a.y - v_b.y) + abs(v_a.z - v_b.z);
}

float Cube::distance(Tile* alpha, Tile* beta) {
	if (glm::dot(
		glm::vec3(alpha->GameComponent::worldMatrix() * glm::vec4(0.0, 1.0, 0.0, 0.0)),
		glm::vec3(beta->GameComponent::worldMatrix() * glm::vec4(0.0, 1.0, 0.0, 0.0))) < -0.5) {
		return 40.0 - distManhattan(
			alpha->GameComponent::worldMatrix() * glm::vec4(0.0, 0.0, 0.0, 1.0),
			beta->GameComponent::worldMatrix() * glm::vec4(0.0, 0.0, 0.0, 1.0));
	}
	return distManhattan(alpha->GameComponent::worldMatrix() * glm::vec4(0.0, 0.0, 0.0, 1.0),
		beta->GameComponent::worldMatrix()* glm::vec4(0.0, 0.0, 0.0, 1.0));
}