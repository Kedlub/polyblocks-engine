// PolyblocksEngine.cpp : Defines the entry point for the application.
//

#include "engine.h"
#include "shader.hpp"
#include "render.hpp"
#include "texture.hpp"
#include "controls.hpp"
#include "objloader.hpp"
#include "world.hpp"
#include "gui.hpp"
#include "global.hpp"
//#include "../libs/glfw-3.3.2/deps/linmath.h"

using namespace std;

int width, height;
GLFWwindow* window;
std::vector<GUIContainer> guiList;


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "[OpenGL] Error: %s\n", description);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE && !inGame)
	{
		double xpos, ypos;
		//getting cursor position
		glfwGetCursorPos(window, &xpos, &ypos);
		int x = (xpos / width) * 800;
		int y = ((height - ypos) / height) * 600;
		//cout << "Cursor Position at (" << x << " : " << y << ")" << endl; - cursor position debugging
		if (!inGame) {
			for (GUIContainer& gui : guiList) {
				BaseWidget* widget = gui.getWidgetByPos(x, y);
				if (ButtonWidget* button = dynamic_cast<ButtonWidget*>(widget)) {
					cout << "Clicked on button with text " << button->text << endl;
					executeAction(button->action);
					break;
				}
			}
		}
	}
}

void showError(string line1, string line2, GLFWwindow *window) {
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	//printText2D(line1.c_str(), 290, 290, 20);
	printText2D(line1.c_str(), (int)(400 - line1.size()*5), 290, 20);
	//printText2D(line2.c_str(), 250, 250, 20);
	printText2D(line2.c_str(), (int)(400 - line2.size()*5), 250, 20);
	glfwSwapBuffers(window);
	glfwPollEvents();
}

int main()
{
	cout << "[engine.cpp] Starting engine...\n";
	//               DONT USE DIACRITICS IN CONSOLE, they don't work	        //
	//         AND NEVER EVER CHANGE LOCALE, IT TOTALLY BREAKS RENDERING        //
	//                   but I don't know why it's happening                    //
	//std::locale cz("Czech");
	//std::locale::global(cz);
	//cout << "[endžin.cpp] Také načítám endžin, a zkouším českou diakritiku\n";
	srand((unsigned int)time(NULL));
	if (!glfwInit()) {
		cout << "[engine.cpp] GLFW Initialization failed." << endl;
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	//OpenGL crashes with these settings, so don't enable them
	//but the opengl tutorial recommends them, so it's weird why it crashes
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	//TODO make better way to choose fullscreen
	//window = glfwCreateWindow(640, 480, "Polyblocks Engine", NULL, NULL);
	//window = glfwCreateWindow(mode->width, mode->height, "Polyblocks Engine", glfwGetPrimaryMonitor(), NULL);
	window = glfwCreateWindow(1280, 740, "Polyblocks Engine", NULL, NULL);
	//int windowW, windowH;
	glfwGetFramebufferSize(window, &width, &height);
	//glfwGetWindowSize(window, &windowW, &windowH);
	glfwSetWindowPos(window, (mode->width - width) / 2, (mode->height - height) / 2);
	if (!window) {
		cout << "[engine.cpp] Failed window initialization! If you're using laptop, make sure you run this on your dedicated graphics card!" << endl;
		exit(EXIT_FAILURE);
	}

	

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetErrorCallback(error_callback);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	//glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
	glfwSetCursorPos(window, width / 2, height / 2);

	printf("[engine.cpp] Loading font renderer...\n");
	initText2D("textures/RobotoMono.dds", "textures/RobotoMonoOutline.dds", "textures/RobotoMonoBold.dds", "textures/RobotoMonoBoldOutline.dds");

	glViewport(0, 0, width, height);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	printText2D("Loading...", 360, 290, 20);
	glfwSwapBuffers(window);
	glfwPollEvents();

	if (!loadAllShaders()) {
		printf("[engine.cpp] Some shaders failed to load! Expect graphic problems");
	}
	/*if (!loadAllTextures()) {
		printf("[engine.cpp] Some textures failed to load!\n");
	}*/
	if (!loadAllModels()) {
		
		printf("[engine.cpp] Some models failed to load!\n");
		while (!glfwWindowShouldClose(window)) {
			drawInfo("Fatal Error!", "Failed to load one of critical models!");
		}
	}
	//World world;
	//world = loadMap("test2");
	
	guiList.push_back(loadRes("mainmenu"));
	//std::this_thread::sleep_for(std::chrono::seconds(1));
	
	/*if (world.isNull || world.getObjectsSize() == 0 || !world.errorText.empty()) {
		printf("[engine.cpp] Failed to load world!");
		if (!world.errorText.empty()) {
			showError("Failed to load world!", world.errorText, window);
		}
		else {
			showError("Failed to load world!", "Did you enter the right name?", window);
		}
	}*/

	//Enable Face Culling, so the insides of meshes won't be rendered
	glEnable(GL_CULL_FACE);
	//Enable Z-index depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//ButtonWidget butt = ButtonWidget(glm::vec2(10,10), glm::vec2(100,50), "Test Button");

	double lastTime = glfwGetTime();
	int nbFrames = 0;
	double msPerFrame = 0;
	int loading = 100;

	while (!glfwWindowShouldClose(window)) {
		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) { // If last printf() was more than 1 sec ago
			// printf and reset timer
			//printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			msPerFrame = 1000.0 / double(nbFrames);
			nbFrames = 0;
			lastTime += 1.0;
		}

		float ratio;
		
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);
		//glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
		//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (inGame) {
			
			//WORLD DOES NOT RENDER! WHY IS IT HAPPENING?? it was working before ;(
			//fixed, it didn't render because of changed Locale, but it's still mystery why it's happening
			if (!currentWorld.isNull && currentWorld.getObjectsSize() != 0) {
				glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				computeMatricesFromInputs(window);
				glm::mat4 ProjectionMatrix = getProjectionMatrix();
				glm::mat4 ViewMatrix = getViewMatrix();
				currentWorld.render(ViewMatrix, ProjectionMatrix);
				printText2D(string("Polyblocks Engine dev build - zkouška českých znaků").c_str(), 10, 580, 20, true);
				printText2D(string(to_string(msPerFrame) + " ms/frame").c_str(), 10, 560, 20, true);
			}
			else {
				//printf("[engine.cpp] Player is inGame but world is null! Something is wrong!");
				if (!currentWorld.errorText.empty()) {
					drawInfo("Error loading world", currentWorld.errorText, false);
				}
				else {
					drawInfo("Error loading world", "Unknown Error", false);
				}
			}

		}
		else {
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			for (GUIContainer& gui : guiList) {
				gui.render();
			}
		}
		

		// Font debugging, it seems it can't render Czech diacritics, even when it's included in the bitmap
		// If someone knows how to fix it, please tell me
		// It works now, i will leave this here in case someone wants to debug font
		/*printText2D("a b c d e f g h i j k l m n o p q r s t u v w x y z", 10, 520, 20, true);
		string str = string("1 2 3 4 5 6 7 8 9 + ě š č ř ž ý á í é");
		//str += (char)158;
		printText2D(str.c_str() , 10, 480, 20, true);*/
		//std::cout << "š = " << +char('š') << "\n";
		int y = 540;
		for(auto var : textBuffer)
		{
			printText2D(var.c_str(), 10, y, 10);
			y -= 20;
		}
		textBuffer.clear();

		

		glfwSwapBuffers(window);
		glfwPollEvents();

		/*GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			printf("[engine.cpp] OpenGL ERROR: %s", err);
		}*/
	}


	cleanupText2D();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
