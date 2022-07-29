#include <iostream>
#include <vector>
#include <windows.h>
#include <algorithm>
#include <ctime>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

//#include "AssetManager.h"

#include "StaticMesh.h"
#include "Camera.h"
#include "Randoms.h"
#include "GameCube.h"
#include "FBO.h"
#include "Texture.h"
#include "GameLevel.h"
#include "StarDome.h"
#include "Artefact.h"
#include "Player.h"

int main() {
	glfwInit();

	glfwInitHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwInitHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwInitHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwSwapInterval(2000);

	GLFWwindow* window = glfwCreateWindow(800, 800, "Main app", NULL, NULL);

	if (window == NULL) {
		std::cout << "Failed to initialize window, terminating" << std::endl;
		glfwTerminate();
		return 0;
	}

	glfwMakeContextCurrent(window);

	gladLoadGL();
	glViewport(0, 0, 800, 800);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);
	//Shader sunShader("sun.vert", "sun.frag");

	stbi_set_flip_vertically_on_load(1.0);

	Texture noiseTexture("perlinNoice.jpg", GL_TEXTURE1);

	Camera camera = Camera(CAMERA_PERSPECTIVE);
	camera.forward = glm::normalize(glm::vec3(-1.0, -1.0, -1.0));
	camera.targetForward = glm::normalize(glm::vec3(-1.0, -1.0, -1.0));
	//camera.forward = glm::normalize(glm::vec3(1.0, 0.0, 0.0));
	camera.speed = 1.0;

	Camera shadowCamera = Camera(CAMERA_ORTHOGRAPHIC);
	shadowCamera.position = camera.position - camera.localRight() + camera.localUp();
	shadowCamera.forward = -glm::normalize(shadowCamera.position);
	int shadowmapResolution = 2048;
	int indexResolution = 512;

	/*GLuint indexFBO; 
	glGenFramebuffers(1, &indexFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, indexFBO);
	GLuint indexmapDepthRBO;
	glGenRenderbuffers(1, &indexmapDepthRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, indexmapDepthRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, indexResolution, indexResolution);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, indexmapDepthRBO);
	GLuint indexmapRBO;
	glGenRenderbuffers(1, &indexmapRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, indexmapRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB16UI, indexResolution, indexResolution);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, indexmapRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);*/


	int cubeSide = 3;

	long long seed = std::time(nullptr);
	RandomGenerator generator(seed);
	std::cout << "SEED: " << seed << std::endl;

	std::cout << "Loading assets...\n";
	GameLevel mainLevel(&generator);
	mainLevel.mainCamera = &camera;
	mainLevel.loadShader("TerrainShader", "default.vert", "default.frag");
	mainLevel.loadShader("DepthShader", "default.vert", "shadow.frag");
	mainLevel.loadShader("TexturedShader", "default.vert", "textured.frag");
	mainLevel.loadShader("TexturedDepthShader", "default.vert", "texturedDepth.frag");
	mainLevel.loadShader("StarShader", "default.vert", "bloomingSphere.frag");
	std::cout << "Updooting shaders...\n";
	mainLevel.getShader("TexturedShader")->activate();
	glUniform1i(mainLevel.getShader("TexturedShader")->uniform("atlas"), 3);
	mainLevel.getShader("TexturedDepthShader")->activate();
	glUniform1i(mainLevel.getShader("TexturedDepthShader")->uniform("atlas"), 3);
	std::cout << "Updoot complete.\n";

	// GAMEPLAY MODELS //

	mainLevel.loadAsset("TreeMesh.txt", true, 0.05);
	mainLevel.getMesh("TreeMesh.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");

	mainLevel.loadAsset("Flag.txt", true, 0.3);
	mainLevel.getMesh("Flag.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("Flag.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	mainLevel.loadAsset("pilar.txt", true, 0.1);
	mainLevel.getMesh("pilar.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("pilar.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	mainLevel.loadAsset("MushroomMesh.txt", true, 0.05 * 0.6);
	mainLevel.getMesh("MushroomMesh.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");

	mainLevel.loadAsset("RockMesh.txt", true, 0.05 * 0.4);
	mainLevel.getMesh("RockMesh.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");

	mainLevel.loadAsset("ZSquareMesh.txt", false, 0.1 / cubeSide);
	mainLevel.getMesh("ZSquareMesh.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TexturedShader");
	mainLevel.getMesh("ZSquareMesh.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("TexturedDepthShader");

	mainLevel.loadAsset("YSquareMesh.txt", false, 1.0);
	mainLevel.getMesh("YSquareMesh.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TexturedShader");
	mainLevel.getMesh("YSquareMesh.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("TexturedDepthShader");

	mainLevel.loadAsset("StarSquare.txt", false, 1.0);
	mainLevel.getMesh("StarSquare.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("StarShader");

	mainLevel.loadAsset("Camp.txt", true, 0.7);
	mainLevel.getMesh("Camp.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("Camp.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	// HEROES //

	mainLevel.loadAsset("Wizard.txt", true, 0.23);
	mainLevel.getMesh("Wizard.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("Wizard.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	mainLevel.loadAsset("Berserk.txt", true, 0.23);
	mainLevel.getMesh("Berserk.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("Berserk.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	mainLevel.loadAsset("Elsa.txt", true, 0.23);
	mainLevel.getMesh("Elsa.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("Elsa.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	mainLevel.loadAsset("Yaga.txt", true, 0.23);
	mainLevel.getMesh("Yaga.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("Yaga.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	mainLevel.loadAsset("Knight.txt", true, 0.23);
	mainLevel.getMesh("Knight.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("Knight.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	mainLevel.loadAsset("Golem.txt", true, 0.38);
	mainLevel.getMesh("Golem.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("Golem.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	// ARTEFACTS //

	mainLevel.loadAsset("Sword.txt", true, 0.4);
	mainLevel.getMesh("Sword.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("Sword.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	mainLevel.loadAsset("Trophy.txt", true, 0.4);
	mainLevel.getMesh("Trophy.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("Trophy.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	mainLevel.loadAsset("Rings.txt", true, 0.4);
	mainLevel.getMesh("Rings.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("Rings.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	mainLevel.loadAsset("Book.txt", true, 0.4);
	mainLevel.getMesh("Book.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("Book.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	mainLevel.loadAsset("Plant.txt", true, 0.4);
	mainLevel.getMesh("Plant.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("Plant.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	mainLevel.loadAsset("Tree.txt", true, 0.6);
	mainLevel.getMesh("Tree.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("Tree.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	mainLevel.loadAsset("Crown.txt", true, 0.4);                    
	mainLevel.getMesh("Crown.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("Crown.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	// POINTERS //

	mainLevel.loadAsset("ArrowWithCircle.txt", true, 0.8);
	mainLevel.getMesh("ArrowWithCircle.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("ArrowWithCircle.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	mainLevel.loadAsset("MagicalStuff.txt", true, 0.8);
	mainLevel.getMesh("MagicalStuff.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("MagicalStuff.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	const char* atlasName = "wasd.png";
	mainLevel.loadAtlas(atlasName, GL_TEXTURE3, GL_RGBA);

	std::cout << "Asset upload finished\nDefining regions...\n";

	mainLevel.getAtlas(atlasName)->defineRegion("w",
		AtlasRegion(glm::vec2(0.0, 0.5), glm::vec2(0.5, 0.5)));
	mainLevel.getAtlas(atlasName)->defineRegion("s",
		AtlasRegion(glm::vec2(0.5, 0.5), glm::vec2(0.5, 0.5)));
	mainLevel.getAtlas(atlasName)->defineRegion("a",
		AtlasRegion(glm::vec2(0.0, 0.0), glm::vec2(0.5, 0.5)));
	mainLevel.getAtlas(atlasName)->defineRegion("d",
		AtlasRegion(glm::vec2(0.5, 0.0), glm::vec2(0.5, 0.5)));

	std::cout << "Regions defined\nMarking textures...\n";

	Texture atlasTexture = Texture("wasd.png", GL_TEXTURE3, GL_RGBA);
	glActiveTexture(GL_TEXTURE3);
	mainLevel.getAtlas(atlasName)->bind();
	atlasTexture.bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	mainLevel.getShader("TexturedShader")->activate();
	glUniform1i(mainLevel.getShader("TexturedShader")->uniform("atlas"), 3);
	mainLevel.getShader("TexturedDepthShader")->activate();
	glUniform1i(mainLevel.getShader("TexturedDepthShader")->uniform("atlas"), 3);

	glActiveTexture(GL_TEXTURE0);
	FBO shadowFBO = FBO(shadowmapResolution, shadowmapResolution, GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT);
	mainLevel.getShader("TerrainShader")->activate();
	glBindTexture(GL_TEXTURE_2D, shadowFBO.textureID);
	glUniform1i(mainLevel.getShader("TerrainShader")->uniform("shadowmap"), 0);
	shadowCamera.pushMatrices(*mainLevel.getShader("TerrainShader"), true);

	glActiveTexture(GL_TEXTURE1);
	noiseTexture.bind();
	glUniform1i(mainLevel.getShader("TerrainShader")->uniform("noiseMap"), 1);
	std::cout << "Texture markings finished\nGenerating level...\n";

	Cube testCube(cubeSide, generator, &mainLevel);
	testCube.assignParent();
	testCube.parent = nullptr;
	testCube.transform = glm::scale(glm::vec3(1.0 / testCube.side));
	std::vector<const char*> artefactNames = {"Sword.txt", "Trophy.txt", "Rings.txt", "Book.txt", "Plant.txt", "Tree.txt"};
	std::vector<Artefact> artefacts;
	for (const char* name : artefactNames) {
		artefacts.push_back(Artefact(name, &mainLevel));
	}
	Tile* playerTile = testCube.bottom.tiles[testCube.side / 2][testCube.side / 2];
	for (int i = 0; i < artefacts.size(); i++) {
		Tile* avoidanceTile = playerTile;
		if (i > 0) {
			avoidanceTile = (Tile*)artefacts[i - 1].parent;
		}
		artefacts[i].decideOnPosition(avoidanceTile, &generator);
	}
	std::vector<Player*> players;
	for (GameSide* side : testCube.sides()) {
		players.push_back(new Player(&mainLevel, players.size()));
		side->tiles[side->size / 2][side->size / 2]->attach(players[players.size() - 1]);
		players[players.size() - 1]->bot = true;
		players[players.size() - 1]->stage = PLAYER_WAITING_FOR_TURN;
	}

	players[0]->bot = false;

	for (int artefactIndex = 0; artefactIndex < artefacts.size(); artefactIndex++) {
		players[artefactIndex]->bountylist.push_back(&artefacts[artefactIndex]);
		artefacts[artefactIndex].decideOnPosition((Tile*)players[artefactIndex]->parent, &generator);
	}
	StaticMeshComponent* activeCrown = new StaticMeshComponent();
	activeCrown->mesh = mainLevel.getMesh("Crown.txt");
	StaticMeshComponent* selectionArrow = new StaticMeshComponent();
	selectionArrow->mesh = mainLevel.getMesh("ArrowWithCircle.txt");
	StaticMeshComponent* artefactPointer = new StaticMeshComponent();
	artefactPointer->mesh = mainLevel.getMesh("MagicalStuff.txt");
	std::cout << "Level generated successfully, defining render features...\n";
	
	StarDome stars(&mainLevel, 1000, &generator);

	camera.armLength = 2.0;
	camera.cinematic = true;
	camera.armStrength = 8.0;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	//glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);

	std::cout << "Preparings are complete, entering main game loop\n";
	float time = glfwGetTime();
	float startTime = time;
	int tick = 0;
	float debugPrintTime = 0;
	int previousLMBState = GLFW_RELEASE;
	float playerActionTime = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		float newTime = glfwGetTime();
		float deltaTime = newTime - time;
		time = newTime;
		tick++;
		glfwSetWindowTitle(window, ("Game window, FPS:" + std::to_string(1.0 / deltaTime)).c_str());

		testCube.update(deltaTime);
		activeCrown->transform = glm::mat4(
			glm::vec4(sin(time), 0.0, cos(time), 0.0),
			glm::vec4(0.0, 1.0, 0.0, 0.0),
			glm::vec4(-cos(time), 0.0, sin(time), 0.0),
			glm::vec4(0.0, 0.3, 0.0, 1.0)
		);
		selectionArrow->transform = glm::mat4(
			glm::vec4(sin(-time), 0.0, cos(-time), 0.0),
			glm::vec4(0.0, 1.0, 0.0, 0.0),
			glm::vec4(-cos(-time), 0.0, sin(-time), 0.0),
			glm::vec4(0.0, 0.3, 0.0, 1.0)
		);

		artefactPointer->transform = glm::mat4(
			glm::vec4(sin(-time * 1.5), 0.0, cos(-time * 1.5), 0.0),
			glm::vec4(0.0, 1.0, 0.0, 0.0),
			glm::vec4(-cos(-time * 1.5), 0.0, sin(-time * 1.5), 0.0),
			glm::vec4(0.0, 0.15 + 0.06 * sin(time), 0.0, 1.0)
		);

		mainLevel.getShader("DepthShader")->activate();
		glUniform1f(mainLevel.getShader("DepthShader")->uniform("time"), time);
		mainLevel.getShader("TexturedDepthShader")->activate();
		glUniform1f(mainLevel.getShader("TexturedDepthShader")->uniform("time"), time);
		mainLevel.getShader("TerrainShader")->activate();
		glUniform1f(mainLevel.getShader("TerrainShader")->uniform("time"), time);

		shadowCamera.position = glm::mix(shadowCamera.position, 
			camera.position - camera.localRight() + camera.localUp(), 
			0.1 * deltaTime);
		float sunStart = 15.0;
		shadowCamera.position = glm::mat3(
			glm::vec3(1.0, 1.0, 1.0),
			glm::vec3(1.0, 0.0, -1.0),
			glm::vec3(-1.0, 1.0, -1.0)
		) * glm::vec3(sin((time + sunStart) * 0.02), cos((time + sunStart) * 0.02), 0.0);
		shadowCamera.position = glm::normalize(shadowCamera.position);
		shadowCamera.forward = -glm::normalize(shadowCamera.position);
		mainLevel.getShader("TerrainShader")->activate();
		shadowCamera.pushMatrices(*mainLevel.getShader("TerrainShader"), true);
		glm::vec3 lightV = glm::normalize(shadowCamera.position);
		glUniform3f(mainLevel.getShader("TerrainShader")->uniform("lightDirection"),
			lightV.x, lightV.y, lightV.z);
		mainLevel.getShader("TexturedShader")->activate();
		glUniform3f(mainLevel.getShader("TexturedShader")->uniform("lightDirection"),
			lightV.x, lightV.y, lightV.z);
		mainLevel.getShader("StarShader")->activate();
		glUniform1f(mainLevel.getShader("StarShader")->uniform("time"), time);

		shadowFBO.bind();
		glViewport(0, 0, shadowmapResolution, shadowmapResolution);
		glClear(GL_DEPTH_BUFFER_BIT);
		
		testCube.draw(shadowCamera, RENDER_DEPTH);
		shadowFBO.unbind();

		//glBindFramebuffer(GL_FRAMEBUFFER, indexFBO);
		//glViewport(0, 0, indexResolution, indexResolution);
		//glClearColor(10000, 10000, 10000, 1);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		////testCube.draw(camera, RENDER_INDICES);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		int windowWidth, windowHeight;
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		glViewport(0, 0, windowWidth, windowHeight);
		camera.displayWidth = windowWidth;
		camera.displayHeight = windowHeight;

		glClearColor(0.2, 0.5, 0.7, 1.0);
		glClearColor(0.1, 0.1, 0.15, 1.0);
		mainLevel.getShader("StarShader")->activate();
		glUniform4f(mainLevel.getShader("StarShader")->uniform("skyColor"), 0.1, 0.1, 0.15, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		testCube.draw(camera, RENDER_MAIN_PASS);
		stars.draw(camera, RENDER_MAIN_PASS);

		glfwSwapBuffers(window);

		Player* activePlayer = nullptr;
		for (int playerIndex = 0; playerIndex < players.size(); playerIndex++) {
			if (players[playerIndex]->stage == PLAYER_FINISHED_TURN || 
				players[playerIndex]->stage == PLAYER_FINISHED_GAME) continue;
			activePlayer = players[playerIndex];
			if (activePlayer->stage == PLAYER_WAITING_FOR_TURN) {
				activePlayer->stage++;
			}
			break;
		}
		if (!activePlayer) {
			for (int playerIndex = 0; playerIndex < players.size(); playerIndex++) {
				if (players[playerIndex]->stage == PLAYER_FINISHED_TURN) players[playerIndex]->stage = PLAYER_WAITING_FOR_TURN;
			}
			for (int playerIndex = 0; playerIndex < players.size(); playerIndex++) {
				if (players[playerIndex]->stage == PLAYER_WAITING_FOR_TURN) {
					players[playerIndex]->stage = PLAYER_PREPARING_FLIP;
					activePlayer = players[playerIndex];
					break;
				}
			}
		}

		// If all players finished the game...
		bool gameFinished = true;
		for (int playerIndex = 0; playerIndex < players.size(); playerIndex++) {
			if (players[playerIndex]->bountylist.size() > 0) gameFinished = false;
		}
		if (gameFinished) {
			break;
		}

		if (activePlayer->bot && time - playerActionTime > 0.5 && time - startTime > 3.0) {
			for (int playerIndex = 0; playerIndex < players.size(); playerIndex++) {
				players[playerIndex]->shouldSmoothMovement = false;
			}
			activePlayer->makeTurn(generator);
			playerActionTime = time;
		}

		if (activeCrown->parent != activePlayer) {
			activeCrown->attachTo(activePlayer);
		}

		debugPrintTime = time;
		double xPix, yPix;
		glfwGetCursorPos(window, &xPix, &yPix);
		float xPos = 2.0 * xPix / windowWidth - 1.0;
		float yPos = 2.0 * yPix / windowHeight - 1.0;
		Tile* traceResult = testCube.checkPixel(&camera, glm::vec2(xPos, -yPos));
		testCube.resetHighlights(false);
		selectionArrow->detach();
		artefactPointer->detach();
		if (!activePlayer->bot) {
			if (traceResult && glfwGetMouseButton(window, 1) != GLFW_PRESS) {
				if (glfwGetMouseButton(window, 0) == GLFW_PRESS && glfwGetMouseButton(window, 0) != previousLMBState) {
					testCube.resetHighlights(true);
					traceResult->highlight = 1.0;
					testCube.activeTile = traceResult;
					if (activePlayer->stage == PLAYER_PREPARING_RUN && ((Tile*)activePlayer->parent)->checkAccess(traceResult)) {
						activePlayer->shouldSmoothMovement = true;
						traceResult->attach(activePlayer);
						activePlayer->stage = PLAYER_FINISHED_TURN;
						playerActionTime = time;
						testCube.resetHighlights(true);
						activePlayer->checkArtefacts();
					}
					if (activePlayer->stage == PLAYER_PREPARING_RUN) {
						testCube.resetHighlights(true);
					}
				}
				else {
					if (activePlayer->stage != PLAYER_WAITING_FOR_TURN && activePlayer->stage != PLAYER_FINISHED_TURN &&
						(activePlayer->stage == PLAYER_PREPARING_FLIP || ((Tile*)activePlayer->parent)->checkAccess(traceResult))) {
						traceResult->highlight = std::max(0.5f, traceResult->highlight);
						if (activePlayer->bot == false && activePlayer->stage == PLAYER_PREPARING_RUN) {
							selectionArrow->attachTo(traceResult);
						}
					}
				}
			}
			if (glfwGetMouseButton(window, 0) == GLFW_PRESS && !traceResult) {
				testCube.resetHighlights(true);
				testCube.activeTile = nullptr;
			}
			for (int key : {GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D}) {
				if (glfwGetKey(window, key) != GLFW_PRESS || activePlayer->stage != PLAYER_PREPARING_FLIP) continue;
				for (int playerIndex = 0; playerIndex < players.size(); playerIndex++) {
					players[playerIndex]->shouldSmoothMovement = false;
				}
				activePlayer->shouldSmoothMovement = false;
				if (activePlayer->bountylist.size() > 0) activePlayer->stage = PLAYER_PREPARING_RUN;
				else activePlayer->stage = PLAYER_FINISHED_TURN;
				playerActionTime = time;
				testCube.receveInput(key, true);
			}
		}
		if (activePlayer && activePlayer->bountylist.size() > 0) {
			activePlayer->objective()->attach(artefactPointer);
		}
		previousLMBState = glfwGetMouseButton(window, 0);
		camera.operateInputs(window, deltaTime);
		glfwPollEvents();
	}
	shadowFBO.destroy();
	glfwTerminate();
}