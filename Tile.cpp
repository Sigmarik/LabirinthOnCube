#include "Tile.h"

template <typename T>
T max(T a, T b) {
	if (a < b) return b;
	return a;
}

template <typename T>
T min(T a, T b) {
	if (a < b) return a;
	return b;
}

Tile::Tile(int routs, GameLevel* level, int _id) {
	openedRouts = routs;
	parentLevel = level;
	id = _id;
	mesh = new StaticMesh();
	mesh->updateBuffers();
	mesh->shaders[RENDER_MAIN_PASS] = parentLevel->getShader("TerrainShader");
	mesh->shaders[RENDER_DEPTH] = parentLevel->getShader("DepthShader");
	wPlane = SpriteComponent(parentLevel, parentLevel->getAtlas("wasd.png"), "w");
	sPlane = SpriteComponent(parentLevel, parentLevel->getAtlas("wasd.png"), "s");
	aPlane = SpriteComponent(parentLevel, parentLevel->getAtlas("wasd.png"), "a");
	dPlane = SpriteComponent(parentLevel, parentLevel->getAtlas("wasd.png"), "d");
	float centreShift = 0.3;
	float yShift = 0.2;
	wPlane.hidden = true;
	sPlane.hidden = true;
	aPlane.hidden = true;
	dPlane.hidden = true;
	//attach(&wPlane);
	//attach(&sPlane);
	//attach(&aPlane);
	//attach(&dPlane);//*/
}

float Tile::roadTransition(float k) {
	return 1.0 - pow(k, 5.0);
}

float Tile::roadCoefficient(float x, float y) {
	if (openedRouts == 0) return 0.0;
	float roadWidth = 0.2;
	float coef = roadTransition(min((float)1.0, sqrt(x * x + y * y) / (roadWidth * 2.0f)));
	// Positive X = [W]
	if (openedRouts & 1 && x > 0.0) {
		float newK = roadTransition(min(abs(y) / roadWidth, (float)1.0));
		coef = max(coef, newK);
	}
	// Negative X = [S]
	if (openedRouts & 2 && x < 0.0) {
		float newK = roadTransition(min(abs(y) / roadWidth, (float)1.0));
		coef = max(coef, newK);
	}
	// Positive Y = [D]
	if (openedRouts & 4 && y > 0.0) {
		float newK = roadTransition(min(abs(x) / roadWidth, (float)1.0));
		coef = max(coef, newK);
	}
	// Negative Y = [A]
	if (openedRouts & 8 && y < 0.0) {
		float newK = roadTransition(min(abs(x) / roadWidth, (float)1.0));
		coef = max(coef, newK);
	}
	return min((float)1.0, coef);
}

void Tile::generateGeometry(RandomGenerator& generator) {
	mesh = new StaticMesh();
	mesh->shaders[RENDER_MAIN_PASS] = parentLevel->getShader("TerrainShader");
	mesh->shaders[RENDER_DEPTH] = parentLevel->getShader("DepthShader");
	mesh->updateBuffers();
	mesh->clear();
	int gridSide = 20;
	float noiseRoof = 2.0 / (float)gridSide;
	float meanedX = 0.0, meanedZ = 0.0;
	mesh->vertices = std::vector<Vertex>(2 * gridSide * gridSide, Vertex());
	std::vector<const char*> featureMeshNames = {"TreeMesh.txt", "TreeMesh.txt", "RockMesh.txt", "MushroomMesh.txt"};
	for (int i = 0; i < gridSide; i++) {
		for (int j = 0; j < gridSide; j++) {
			float relativeX = (float)i / (float)(gridSide - 1) - 0.5;
			float relativeZ = (float)j / (float)(gridSide - 1) - 0.5;
			float relativeY = ((float)generator.range(0, 1e3) / (float)1e3) * noiseRoof;
			if (i == 0 || j == 0 || i + 1 == gridSide || j + 1 == gridSide) {
				relativeY = 0.0;
			}
			float roadAmount = roadCoefficient(relativeX, relativeZ) * 0.9;
			relativeY *= 1.0 - roadAmount;
			glm::vec4 color = glm::vec4(relativeX * 2, relativeY / noiseRoof, relativeZ * 2, roadAmount);
			mesh->vertices[i * gridSide + j] = Vertex(
				glm::vec3(relativeX, relativeY, relativeZ),
				glm::vec2((float)i / gridSide, (float)j / gridSide),
				color
			);
			float gradient = max(0.0, 1.0 - glm::length(glm::vec2(relativeX, relativeY)) * 2.0);
			float downwardsY = ((float)generator.range(0, 2e3) / (float)1e3) * 0.1;
			mesh->vertices[gridSide * gridSide + i * gridSide + j] = Vertex(
				glm::vec3(relativeX, -downwardsY * gradient, relativeZ),
				glm::vec2((float)i / gridSide, (float)j / gridSide),
				glm::mix(glm::vec4(0.3, 0.3, 0.31, 1.0), glm::vec4(0.9, 0.9, 1.0, 1.0), downwardsY * gradient * 10.0)
			);
			if (i != 0 && i + 1 != gridSide && j != 0 && j + 1 != gridSide && 
				roadAmount <= 0.3 && generator.range(0, 100) < 20) {
				if (featureMeshNames.size() == 0) {
					std::cout << "Empty feature set!" << std::endl;
				}
				int featureIndex = generator.range(0, featureMeshNames.size());
				const char* featureName = featureMeshNames[featureIndex];
				StaticMeshComponent feature;
				feature.mesh = parentLevel->getMesh(featureName);
				float angle = (float)generator.range(0, 1000) / 100.0;
				float scale = (float)generator.range(75, 125) / 100;
				feature.transform = glm::mat4(1.0);
				feature.transform = glm::translate(feature.transform, glm::vec3(relativeX, relativeY - 0.03, relativeZ));
				feature.transform = glm::scale(feature.transform, glm::vec3(scale));
				feature.transform = glm::rotate(feature.transform, angle, glm::vec3(0.0, 1.0, 0.0));
				//attach(*treeMesh);
				mesh->attach(feature.mesh, feature.transform);
			}
			meanedX += abs(relativeX);
			meanedZ += abs(relativeZ);
		}
	}
	//std::cout << "Meaned: " << meanedX / vertices.size() << ' ' << meanedZ / vertices.size() << std::endl;
	for (int i = 0; i < gridSide - 1; i++) {
		for (int j = 0; j < gridSide - 1; j++) {
			if (generator.range(0, 100) < 50) {
				mesh->faces.push_back((i + 0) * gridSide + (j + 0));
				mesh->faces.push_back((i + 1) * gridSide + (j + 0));
				mesh->faces.push_back((i + 1) * gridSide + (j + 1));

				mesh->faces.push_back((i + 0) * gridSide + (j + 0));
				mesh->faces.push_back((i + 1) * gridSide + (j + 1));
				mesh->faces.push_back((i + 0) * gridSide + (j + 1));

				mesh->faces.push_back(gridSide * gridSide + (i + 1) * gridSide + (j + 0));
				mesh->faces.push_back(gridSide * gridSide + (i + 0) * gridSide + (j + 0));
				mesh->faces.push_back(gridSide * gridSide + (i + 1) * gridSide + (j + 1));

				mesh->faces.push_back(gridSide * gridSide + (i + 1) * gridSide + (j + 1));
				mesh->faces.push_back(gridSide * gridSide + (i + 0) * gridSide + (j + 0));
				mesh->faces.push_back(gridSide * gridSide + (i + 0) * gridSide + (j + 1));
			}
			else {
				mesh->faces.push_back((i + 0) * gridSide + (j + 0));
				mesh->faces.push_back((i + 1) * gridSide + (j + 0));
				mesh->faces.push_back((i + 0) * gridSide + (j + 1));

				mesh->faces.push_back((i + 1) * gridSide + (j + 0));
				mesh->faces.push_back((i + 1) * gridSide + (j + 1));
				mesh->faces.push_back((i + 0) * gridSide + (j + 1));

				mesh->faces.push_back(gridSide * gridSide + (i + 1) * gridSide + (j + 0));
				mesh->faces.push_back(gridSide * gridSide + (i + 0) * gridSide + (j + 0));
				mesh->faces.push_back(gridSide * gridSide + (i + 0) * gridSide + (j + 1));

				mesh->faces.push_back(gridSide * gridSide + (i + 1) * gridSide + (j + 1));
				mesh->faces.push_back(gridSide * gridSide + (i + 1) * gridSide + (j + 0));
				mesh->faces.push_back(gridSide * gridSide + (i + 0) * gridSide + (j + 1));
			}
		}
	}
	mesh->toSharp();
	mesh->recalculateNormals();
	mesh->assignInt(id);
	mesh->updateBuffers();
	attach(&wPlane);
	attach(&sPlane);
	attach(&aPlane);
	attach(&dPlane);
	//std::cout << "Generated " << mesh->vertices.size() << " vertices and " << mesh->faces.size() << " faces.";
}

void Tile::draw(Camera& camera, int shaderSlot) {
	bool validShader = !(mesh->shaders.find(shaderSlot) == mesh->shaders.end() || !mesh->shaders[shaderSlot]);
	//std::cout << "Draw was called on\n" << mesh->vertices.size() << std::endl;
	if (validShader) {
		mesh->shaders[shaderSlot]->activate();
		glUniform1f(mesh->shaders[shaderSlot]->uniform("highlight"), highlight);
	}
	if (highlight >= 0.999) {
		wPlane.hidden = false;
		sPlane.hidden = false;
		aPlane.hidden = false;
		dPlane.hidden = false;
	}
	//std::cout << "Called tile draw.\n";
	StaticMeshComponent::draw(camera, shaderSlot);
	wPlane.hidden = true;
	sPlane.hidden = true;
	aPlane.hidden = true;
	dPlane.hidden = true;
	if (validShader) {
		mesh->shaders[shaderSlot]->activate();
		glUniform1f(mesh->shaders[shaderSlot]->uniform("highlight"), 0.0);
	}
}

float cross(glm::vec2 a, glm::vec2 b) {
	return a.x * b.y - a.y * b.x;
}

glm::vec4 circularize(glm::vec4 position) {
	return glm::vec4(glm::vec3(position) / (float)pow(length(glm::vec3(position)), 0.13), 1.0);
}

bool checkTriangle(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 point) {
	bool checkA = cross(B - A, point - A) * cross(point - A, C - A) >= 0;
	bool checkB = cross(A - B, point - B) * cross(point - B, C - B) >= 0;
	bool checkC = cross(A - C, point - C) * cross(point - C, B - C) >= 0;
	return checkA && checkB && checkC;
}

Tile* Tile::checkPixel(Camera* camera, glm::vec2 pixel) {
	glm::mat4 worldMat = worldMatrix();
	glm::mat4 cameraMatrix = camera->matrix();
	glm::vec4 leftTopC = cameraMatrix * circularize(worldMat * glm::vec4(glm::vec3(-.5, .0, 0.5) * 1.0f, 1.0));
	glm::vec4 leftBtmC = cameraMatrix * circularize(worldMat * glm::vec4(glm::vec3(-.5, .0, -.5) * 1.0f, 1.0));
	glm::vec4 rghtTopC = cameraMatrix * circularize(worldMat * glm::vec4(glm::vec3(0.5, .0, 0.5) * 1.0f, 1.0));
	glm::vec4 rghtBtmC = cameraMatrix * circularize(worldMat * glm::vec4(glm::vec3(0.5, .0, -.5) * 1.0f, 1.0));
	leftTopC /= leftTopC.w;
	leftBtmC /= leftBtmC.w;
	rghtTopC /= rghtTopC.w;
	rghtBtmC /= rghtBtmC.w;
	if (checkTriangle(leftTopC, leftBtmC, rghtTopC, pixel) ||
		checkTriangle(rghtBtmC, leftBtmC, rghtTopC, pixel)) return this;
	else return nullptr;
}

glm::mat4 Tile::worldMatrix() {
	return curentWorld;
}

template <typename T>
T newMix(T a, T b, double x) {
	if (x < 0.0) return a;
	if (x > 1.0) return b;
	return a * (float)(1.0 - x) + b * (float)x;
}

void Tile::update(float deltaTime) {
	//if (curentWorld == glm::mat4(-100.0)) curentWorld = GameComponent::worldMatrix();
	glm::mat4 newTransform = GameComponent::worldMatrix();
	curentWorld = newMix(curentWorld, newTransform, (double)7.0 * deltaTime);
	GameComponent::update(deltaTime);
	float centreShift = 0.3;
	float yShift = 0.3;
	wPlane.transform[3] = glm::vec4(glm::inverse(glm::mat3(transform)) * glm::vec3(centreShift, yShift, 0.0), 1.0);
	sPlane.transform[3] = glm::vec4(glm::inverse(glm::mat3(transform)) * glm::vec3(-centreShift, yShift, 0.0), 1.0);
	aPlane.transform[3] = glm::vec4(glm::inverse(glm::mat3(transform)) * glm::vec3(0.0, yShift, -centreShift), 1.0);
	dPlane.transform[3] = glm::vec4(glm::inverse(glm::mat3(transform)) * glm::vec3(0.0, yShift, centreShift), 1.0); 
}

std::vector<glm::vec4> Tile::getConnections() {
	std::vector<glm::vec4> answer;
	float centreShift = 0.5;
	float yShift = 0.0;
	if (openedRouts & (1 << 0)) answer.push_back(GameComponent::worldMatrix() * glm::vec4(glm::inverse(glm::mat3(transform)) * glm::vec3(centreShift, yShift, 0.0), 1.0));
	if (openedRouts & (1 << 1)) answer.push_back(GameComponent::worldMatrix() * glm::vec4(glm::inverse(glm::mat3(transform)) * glm::vec3(-centreShift, yShift, 0.0), 1.0));
	if (openedRouts & (1 << 3)) answer.push_back(GameComponent::worldMatrix() * glm::vec4(glm::inverse(glm::mat3(transform)) * glm::vec3(0.0, yShift, -centreShift), 1.0));
	if (openedRouts & (1 << 2)) answer.push_back(GameComponent::worldMatrix() * glm::vec4(glm::inverse(glm::mat3(transform)) * glm::vec3(0.0, yShift, centreShift), 1.0));
	return answer;
}

bool Tile::checkAccess(Tile* tile) {
	for (glm::vec3 pointA : getConnections()) {
		for (glm::vec3 pointB : tile->getConnections()) {
			if (glm::distance(pointA, pointB) <= getScale() * 0.001) return true;
		}
	}
	return false;
}