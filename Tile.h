#ifndef TILE_CLASS_H
#define TILE_CLASS_H

#include "StaticMesh.h"
#include "Randoms.h"
#include "tform.h"
#include "StaticMeshComponent.h"
#include "SpriteComponent.h"
#include "GameLevel.h"

class Tile : public StaticMeshComponent {
private:
	float roadTransition(float k);
	float roadCoefficient(float x, float y);
public:
	int openedRouts = 0;
	float updateTime = -1.0;
	float highlight = 0.0;
	glm::mat4 curentWorld = glm::mat4(1.0);
	int id = 0;
	GameLevel* parentLevel = nullptr;
	SpriteComponent wPlane, sPlane, aPlane, dPlane;
	Tile(int routs = 0, GameLevel* level = nullptr, int _id = 0);
	Tile* checkPixel(Camera* camera, glm::vec2 pixel);
	void generateGeometry(RandomGenerator& generator);
	glm::mat4 worldMatrix() override;
	std::vector<glm::vec4> getConnections();
	bool checkAccess(Tile* tile);
	void update(float deltaTime) override;
	void draw(Camera& camera, int shaderSlot) override;
};

#endif