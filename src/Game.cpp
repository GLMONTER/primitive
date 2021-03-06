#include "Game.hpp"
bool editorEnable = true;
bool startedFlag = false;
Input input;
extern Core core;
static bool hasStartedGame = false;
void start(GLFWwindow* window)
{
	
	if (editorEnable && !startedFlag)
		return;

	for (Model& m : core.models)
	{
		m.spawnPosition = m.position;
	}

	input.init(window);
	
    if(!hasStartedGame)
        core.loadScene("test.txt");

    hasStartedGame = true;
	
	startedFlag = true;
	SoundSystem::stopSound("music");
    SoundSystem::F_PlaySound(core.workingDir + std::string("/rec/sound.mp3"), 0.25f, "music", core.mainCamera.position);
}

void update(std::vector<Model>& modelArray)
{
	glfwSetInputMode(core.window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!startedFlag)
		start(core.window.window);
	if (editorEnable)
		return;

	core.mainCamera.position.z = core.findObject("Cube")->position.z + 15;
	core.mainCamera.position.y = core.findObject("Cube")->position.y + 5;
	core.mainCamera.position.x = core.findObject("Cube")->position.x;

	if (input.isKeyPressed(GLFW_KEY_A))
	{
		core.findObject("Cube")->position.x -= deltaTime * 4;
	}
	if (input.isKeyPressed(GLFW_KEY_D))
	{
		core.findObject("Cube")->position.x += deltaTime * 4;
	}
	if (Model::isCollidedName(*core.findObject("Cube"), "Cube", core.collisionModels, core.models))
	{
        core.findObject("Cube")->position = core.findObject("Cube")->spawnPosition;
	}
	else
	if (Model::isCollidedName(*core.findObject("Cube"), "Win", core.collisionModels, core.models))
	{
		exit(0);
	}
	core.mainCamera.calc(&core.findObject("Cube")->position);
	core.findObject("Cube")->position.z -= deltaTime * 15;

}
