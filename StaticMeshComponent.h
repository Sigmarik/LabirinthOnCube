#ifndef STATIC_MESH_COMPONENT_CLASS_H
#define STATIC_MESH_COMPONENT_CLASS_H

#include "tform.h"
#include "GameLevel.h"
#include "StaticMesh.h"

class StaticMeshComponent : public GameComponent {
public:
	StaticMesh* mesh = nullptr;
	bool shouldWave = false;
	void draw(Camera& camera, int shaderSlot) override;
};

#endif