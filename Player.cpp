#include "Player.h"

GameAction::GameAction() {}

void GameAction::execute() {}

bool GameAction::valid() { return true; }

void GameAction::undo() {}


Player::Player(GameLevel* level, int index, int maxPlayers) {
	parentLevel = level;
	StaticMesh* characterMesh = level->getMesh(level->characterNames[index % (level->characterNames.size())]);
	if (maxPlayers <= 1) {
		transform = glm::mat4(1.0);
	}
	else {
		float angle = 2 * 3.1415926535 * index / maxPlayers;
		angle += 3.1415926535 / maxPlayers;
		float amplitude = 0.3;
		transform = glm::mat4(
			glm::vec4(sin(angle), 0.0, cos(angle), 0.0),
			glm::vec4(0.0, 1.0, 0.0, 0.0),
			glm::vec4(-cos(angle), 0.0, sin(angle), 0.0),
			glm::vec4(sin(angle) * amplitude, 0.0, 
				cos(angle) * amplitude, 1.0)
		);
	}
	mesh = new StaticMesh();
	mesh->shaders[RENDER_MAIN_PASS] = parentLevel->getShader("TerrainShader");
	mesh->shaders[RENDER_DEPTH] = parentLevel->getShader("DepthShader");
	mesh->attach(characterMesh, transform);
	transform = glm::mat4(1.0);
	mesh->updateBuffers();
}

bool Player::checkArtifacts() {
	if (!parent) return false;
	if (bountylist.size() == 0) return false;
	for (GameComponent* child : parent->children) {
		if (child == bountylist[bountylist.size() - 1]) {
			bountylist[bountylist.size() - 1]->collect();
			bountylist.pop_back();
			return true;
		}
	}
}

template <typename T>
T newMix(T a, T b, double x) {
	if (x < 0.0) return a;
	if (x > 1.0) return b;
	return a * (float)(1.0 - x) + b * (float)x;
}

void Player::update(float deltaTime) {
	glm::mat4 newTransform = GameComponent::worldMatrix();
	if (!shouldSmoothMovement)
		curentWorld = newTransform;
	else curentWorld = newMix(curentWorld, newTransform, (double)7.0 * deltaTime);
	GameComponent::update(deltaTime);
}

glm::mat4 Player::worldMatrix() {
	return curentWorld;
}

bool Player::makeTurn(RandomGenerator& generator) {
	if (!bot) return false;
	Cube* cube = (Cube*)parent->parent->parent;
	if (stage == PLAYER_PREPARING_FLIP) {
		shouldSmoothMovement = false;
		std::vector<GameSide*> sides = cube->sides();
		GameSide* chosenSide = sides[generator.range(0, sides.size())];
		int cellI = generator.range(0, chosenSide->size);
		int cellJ = generator.range(0, chosenSide->size);
		std::vector<int> keys = { GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D };
		int key = keys[generator.range(0, keys.size())];
		cube->activeTile = chosenSide->tiles[cellI][cellJ];
		cube->receveInput(key, true);
		stage = PLAYER_PREPARING_RUN;
		return true;
	}
	if (stage == PLAYER_PREPARING_RUN) {
		shouldSmoothMovement = true;
		std::vector<GameSide*> sides = cube->sides();
		Tile* targetTile = nullptr;
		do {
			GameSide* side = sides[generator.range(0, sides.size())];
			int cellI = generator.range(0, side->size);
			int cellJ = generator.range(0, side->size);
			targetTile = side->tiles[cellI][cellJ];
		} while (!((Tile*)parent)->checkAccess(targetTile));
		attachTo(targetTile);
		checkArtifacts();
		stage = PLAYER_FINISHED_TURN;
		return true;
	}
	return false;
}

Artefact* Player::objective() {
	return bountylist[bountylist.size() - 1];
}

FlipTurn::FlipTurn(Tile* _tile, int key) {
	tile = _tile;
	action = key;
}

void FlipTurn::execute() {
	Cube* cube = (Cube*)tile->parent->parent;
	cube->activeTile = tile;
	cube->receveInput(action, true);
}

RunTurn::RunTurn(Player* _player, Tile* tile) {
	player = _player;
	targetTile = tile;
}

void RunTurn::execute() {
	if (!player) return;
	if (!player->parent) {
		player->parent = targetTile;
		targetTile->children.push_back(player);
		return;
	}
	if (!valid()) return;
	player->parent->removeChild(player);
	player->parent = targetTile;
	targetTile->children.push_back(player);
}

bool RunTurn::valid() {
	if (!player) return false;
	if (!player->parent) return true;
	return targetTile->checkAccess((Tile*)player->parent);
}