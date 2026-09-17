#include<iostream>
#include<cstdio>
#include"REngine/REngine.hpp"
#include"REngine/utils/log.hpp"
#include"REngine/utils/settings.hpp"

int main()
{
	setvbuf(stdout, nullptr, _IONBF, 0);

	re::Settings settings;

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(
		static_cast<int>(settings.windowWidth), 
		static_cast<int>(settings.windowHeight),
		"REngine",
		nullptr, nullptr);
	
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