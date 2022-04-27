#ifndef PLAYER_CLASS_H
#define PLAYER_CLASS_H

#define PLAYER_WAITING_FOR_TURN 0
#define PLAYER_PREPARING_FLIP 1
#define PLAYER_PREPARING_RUN 2
#define PLAYER_FINISHED_TURN 3

#include "GameLevel.h"
#include "GameCube.h"
#include "Artefact.h"
#include "Tile.h"

class GameAction {
public:
	GameAction();
	virtual void execute();
	virtual bool valid();
	virtual void undo();
};

class Player : public StaticMeshComponent {
public:
	std::vector<Artefact*> bountylist;
	bool bot = false;
	glm::mat4 curentWorld = glm::mat4(1.0);
	bool shouldSmoothMovement = false;
	int stage = PLAYER_WAITING_FOR_TURN;
	Player(GameLevel* level, int index, int numPlayers = 6);
	bool checkArtifacts();
	glm::mat4 worldMatrix() override;
	void update(float deltaTime) override;
};

class FlipTurn : GameAction {
public:
	Tile* tile = nullptr;
	int action = 0;
	FlipTurn(Tile* _tile, int key);
	void execute() override;
};

class RunTurn : GameAction {
public:
	Player* player;
	Tile* targetTile;
	RunTurn(Player* _player, Tile* tile);
	void execute() override;
	bool valid() override;
};

#endif 