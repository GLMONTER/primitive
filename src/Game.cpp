#include "Game.hpp"
extern bool editorEnable;
static bool startedFlag = false;
Input input;
extern Core core;

void start(GLFWwindow* window)
{
	if (editorEnable && !startedFlag)
		return;

	input.init(window);
	/*
	core.loadScene("test.txt");

	core.findObject("Cube")->position.x = 0;
	//core.findObject("Cube")->position.y = 0;
	core.findObject("Cube")->position.z = 0;
	*/

	startedFlag = true;
}

void update(std::vector<Model>& modelArray)
{
	if (!startedFlag)
		start(core.window.window);
	if (editorEnable)
		return;
	
	core.mainCamera.position.z = core.findObject("Cube")->position.z + 15;
	core.mainCamera.position.y = core.findObject("Cube")->position.y + 3;
	core.mainCamera.position.x = core.findObject("Cube")->position.x;
	static int i = 0;
	i++;
	if (i == 20)
	{
		i = 0;
		std::cout << "col " << Model::isCollided(*core.findObject("Cube"), *core.findObject("Cube5"), core.collisionModels) << std::endl;
	}
	core.mainCamera.calc(&core.findObject("Cube")->position);
	core.findObject("Cube")->position.z -= deltaTime;
}