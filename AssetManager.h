#ifndef ASSET_MANAGER_OBJECT
#define ASSET_MANAGER_OBJECT

#include "StaticMesh.h"

namespace AssetManager {
	StaticMesh* treeMesh = nullptr;
	StaticMesh* rockMesh = nullptr;
	StaticMesh* mushroomMesh = nullptr;
	StaticMesh* zPlaneMesh = nullptr;
	void loadAssets();
}

#endif