#include<iostream>
#include"REngine/REngine.hpp"
#include"REngine/utils/log.hpp"

int main()
{

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(800, 600, "REngine", nullptr, nullptr);
	try {
		re::init(window);
	}
	catch (const std::exception& e) {
		std::cerr << "Fatal error: " << e.what() << std::endl;
		std::cin.get();
		return -1;
	}

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		re::render();
	}



	glfwDestroyWindow(window);
	glfwTerminate();



	return 0;
}