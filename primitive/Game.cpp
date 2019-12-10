#include "Game.hpp"

Input input;
extern Core core;
void start(GLFWwindow* window)
{
	input.init(window);
	core.loadScene("test.txt");
}

void update(std::vector<Model>& modelArray)
{
	if (input.isKeyPressed(GLFW_KEY_Y))
	{
		modelArray[0].position.y += 1 * deltaTime;
	}
}