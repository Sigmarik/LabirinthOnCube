#include <iostream>
#include <vector>
#include <windows.h>
#include <algorithm>

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

	std::cout << "Loading assets...\n";
	GameLevel mainLevel;
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

	mainLevel.loadAsset("TreeMesh.txt", true, 0.05);
	mainLevel.getMesh("TreeMesh.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");

	mainLevel.loadAsset("Flag.txt", true, 0.3);
	mainLevel.getMesh("Flag.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("Flag.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	mainLevel.loadAsset("pilar.txt", true, 0.1);
	mainLevel.getMesh("pilar.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("pilar.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	mainLevel.loadAsset("Wizard.txt", true, 0.23);
	mainLevel.getMesh("Wizard.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("Wizard.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	mainLevel.loadAsset("Sword.txt", true, 0.4);
	mainLevel.getMesh("Sword.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("Sword.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	mainLevel.loadAsset("Trophy.txt", true, 0.4);
	mainLevel.getMesh("Trophy.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("Trophy.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

	mainLevel.loadAsset("Rings.txt", true, 0.4);
	mainLevel.getMesh("Rings.txt")->shaders[RENDER_MAIN_PASS] = mainLevel.getShader("TerrainShader");
	mainLevel.getMesh("Rings.txt")->shaders[RENDER_DEPTH] = mainLevel.getShader("DepthShader");

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

	RandomGenerator generator;

	Cube testCube(cubeSide, generator, &mainLevel);
	testCube.assignParent();
	testCube.parent = nullptr;
	testCube.transform = glm::scale(glm::vec3(1.0 / testCube.side));
	std::vector<Artefact> artefacts;
	artefacts.push_back(Artefact("Sword.txt", &mainLevel));
	artefacts.push_back(Artefact("Trophy.txt", &mainLevel));
	artefacts.push_back(Artefact("Rings.txt", &mainLevel)); 
	Tile* playerTile = testCube.top.tiles[testCube.side / 2][testCube.side / 2];
	for (int i = 0; i < artefacts.size(); i++) {
		artefacts[i].decideOnPosition(playerTile, &generator);
	}
	Player player(&mainLevel, 0, 6);
	playerTile->attach(&player);
	player.stage = PLAYER_PREPARING_FLIP;
	for (int artefactIndex = 0; artefactIndex < artefacts.size(); artefactIndex++)
		player.bountylist.push_back(&artefacts[artefactIndex]);
	std::cout << "Level generated successfully, defining render features...\n";
	
	StarDome stars(&mainLevel, 1000, &generator);

	camera.armLength = 2.0;
	camera.cinematic = true;
	camera.armStrength = 10.0;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);

	std::cout << "Preparings are complete, entering main game loop\n";
	float time = glfwGetTime();
	int tick = 0;
	float debugPrintTime = 0;
	int previousLMBState = GLFW_RELEASE;
	while (!glfwWindowShouldClose(window)) {
		float newTime = glfwGetTime();
		float deltaTime = newTime - time;
		time = newTime;
		tick++;
		glfwSetWindowTitle(window, ("Game window, FPS:" + std::to_string(1.0 / deltaTime)).c_str());

		testCube.update(deltaTime);

		mainLevel.getShader("DepthShader")->activate();
		glUniform1f(mainLevel.getShader("DepthShader")->uniform("time"), time);
		mainLevel.getShader("TexturedDepthShader")->activate();
		glUniform1f(mainLevel.getShader("TexturedDepthShader")->uniform("time"), time);
		mainLevel.getShader("TerrainShader")->activate();
		glUniform1f(mainLevel.getShader("TerrainShader")->uniform("time"), time);

		shadowCamera.position = glm::mix(shadowCamera.position, 
			camera.position - camera.localRight() + camera.localUp(), 
			0.1 * deltaTime);
		shadowCamera.position = glm::mat3(
			glm::vec3(1.0, 1.0, 1.0),
			glm::vec3(1.0, 0.0, -1.0),
			glm::vec3(-1.0, 1.0, -1.0)
		) * glm::vec3(sin(time * 0.02), cos(time * 0.02), 0.0);
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

		debugPrintTime = time;
		double xPix, yPix;
		glfwGetCursorPos(window, &xPix, &yPix);
		float xPos = 2.0 * xPix / windowWidth - 1.0;
		float yPos = 2.0 * yPix / windowHeight - 1.0;
		Tile* traceResult = testCube.checkPixel(&camera, glm::vec2(xPos, -yPos));
		testCube.resetHighlights(false);
		if (traceResult && glfwGetMouseButton(window, 1) != GLFW_PRESS) {
			if (glfwGetMouseButton(window, 0) == GLFW_PRESS && glfwGetMouseButton(window, 0) != previousLMBState) {
				testCube.resetHighlights(true);
				traceResult->highlight = 1.0;
				testCube.activeTile = traceResult;
				if (player.stage == PLAYER_PREPARING_RUN && ((Tile*)player.parent)->checkAccess(traceResult)) {
					player.shouldSmoothMovement = true;
					player.parent->removeChild(&player);
					traceResult->attach(&player);
					player.stage = PLAYER_PREPARING_FLIP;
					testCube.resetHighlights(true);
					player.checkArtifacts();
				}
				if (player.stage == PLAYER_PREPARING_RUN) {
					testCube.resetHighlights(true);
				}
			}
			else {
				if (player.stage != PLAYER_WAITING_FOR_TURN && player.stage != PLAYER_FINISHED_TURN && 
					(player.stage == PLAYER_PREPARING_FLIP || ((Tile*)player.parent)->checkAccess(traceResult)))
					traceResult->highlight = std::max(0.5f, traceResult->highlight);
			}
		}
		if (glfwGetMouseButton(window, 0) == GLFW_PRESS && !traceResult) {
			testCube.resetHighlights(true);
			testCube.activeTile = nullptr;
		}
		for (int key : {GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D}) {
			if (glfwGetKey(window, key) != GLFW_PRESS || player.stage != PLAYER_PREPARING_FLIP) continue;
			player.shouldSmoothMovement = false;
			player.stage = PLAYER_PREPARING_RUN;
			testCube.receveInput(key, true);
		}
		previousLMBState = glfwGetMouseButton(window, 0);
		camera.operateInputs(window, deltaTime);
		glfwPollEvents();
	}
	shadowFBO.destroy();
	glfwTerminate();
}