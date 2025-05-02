#pragma once
#include <GLFW/glfw3.h>

class Input 
{
	public:
		int x = 590;
		bool keys[52];



		void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		void mouse_callback(GLFWwindow* window, double xpos, double ypos);
		void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		void processInput(GLFWwindow* window);
		
		static Input& getInstance() // Singleton is accessed via getInstance()
		{
			static Input instance; // lazy singleton, instantiated on first use
			return instance;
		}

	private:	
		Input(void);
		Input(Input const&); // prevent copies
		void operator=(Input const&); // prevent assignments
	

};


