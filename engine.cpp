// PolyblocksEngine.cpp : Defines the entry point for the application.
//

#include "engine.h"
#include "shader.hpp"
#include "render.hpp"
#include "texture.hpp"
#include "controls.hpp"
#include "objloader.hpp"
#include "world.hpp"
//#include "../libs/glfw-3.3.2/deps/linmath.h"

using namespace std;



static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "[OpenGL] Error: %s\n", description);
}

int main()
{
	cout << "[engine.cpp] Starting engine...\n";
	srand(time(NULL));
	if (!glfwInit()) {
		cout << "[engine.cpp] GLFW Initialization failed." << endl;
		exit(EXIT_FAILURE);
	}
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	//GLFWwindow* window = glfwCreateWindow(640, 480, "Polyblocks Engine", NULL, NULL);
	//GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Polyblocks Engine", glfwGetPrimaryMonitor(), NULL);
	GLFWwindow* window = glfwCreateWindow(1280, 740, "Polyblocks Engine", NULL, NULL);
	int windowW, windowH;
	glfwGetWindowSize(window, &windowW, &windowH);
	glfwSetWindowPos(window, (mode->width - windowW) / 2, (mode->height - windowH) / 2);
	if (!window) {
		cout << "[engine.cpp] Failed window initialization!" << endl;
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

	glfwSetKeyCallback(window, key_callback);
	glfwSetErrorCallback(error_callback);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	//glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
	glfwSetCursorPos(window, windowW / 2, windowH / 2);

	printf("[engine.cpp] Loading font renderer...\n");
	initText2D("textures/RobotoBold.DDS");

	glViewport(0, 0, windowW, windowH);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	printText2D("Loading...", 290, 290, 20);
	glfwSwapBuffers(window);
	glfwPollEvents();


	GLuint programID = LoadShaders("shaders/SimpleVertexShader.vertexshader", "shaders/SimpleFragmentShader.fragmentshader");
	GLuint textureShader = LoadShaders("shaders/UnlitShader.vertexshader", "shaders/UnlitShader.fragmentshader");
	GLuint diffuseShader = LoadShaders("shaders/DiffuseShader.VertexShader", "shaders/DiffuseShader.fragmentshader");

	// One color for each vertex. They were generated randomly.
	static const GLfloat g_color_buffer_data[] = {
		0.583f,  0.771f,  0.014f,
		0.609f,  0.115f,  0.436f,
		0.327f,  0.483f,  0.844f,
		0.822f,  0.569f,  0.201f,
		0.435f,  0.602f,  0.223f,
		0.310f,  0.747f,  0.185f,
		0.597f,  0.770f,  0.761f,
		0.559f,  0.436f,  0.730f,
		0.359f,  0.583f,  0.152f,
		0.483f,  0.596f,  0.789f,
		0.559f,  0.861f,  0.639f,
		0.195f,  0.548f,  0.859f,
		0.014f,  0.184f,  0.576f,
		0.771f,  0.328f,  0.970f,
		0.406f,  0.615f,  0.116f,
		0.676f,  0.977f,  0.133f,
		0.971f,  0.572f,  0.833f,
		0.140f,  0.616f,  0.489f,
		0.997f,  0.513f,  0.064f,
		0.945f,  0.719f,  0.592f,
		0.543f,  0.021f,  0.978f,
		0.279f,  0.317f,  0.505f,
		0.167f,  0.620f,  0.077f,
		0.347f,  0.857f,  0.137f,
		0.055f,  0.953f,  0.042f,
		0.714f,  0.505f,  0.345f,
		0.783f,  0.290f,  0.734f,
		0.722f,  0.645f,  0.174f,
		0.302f,  0.455f,  0.848f,
		0.225f,  0.587f,  0.040f,
		0.517f,  0.713f,  0.338f,
		0.053f,  0.959f,  0.120f,
		0.393f,  0.621f,  0.362f,
		0.673f,  0.211f,  0.457f,
		0.820f,  0.883f,  0.371f,
		0.982f,  0.099f,  0.879f,
		0.583f,  0.771f,  0.014f,
		0.609f,  0.115f,  0.436f,
		0.327f,  0.483f,  0.844f,
		0.822f,  0.569f,  0.201f,
		0.435f,  0.602f,  0.223f,
		0.310f,  0.747f,  0.185f,
		0.597f,  0.770f,  0.761f,
		0.559f,  0.436f,  0.730f,
		0.359f,  0.583f,  0.152f,
		0.483f,  0.596f,  0.789f,
		0.559f,  0.861f,  0.639f,
		0.195f,  0.548f,  0.859f,
		0.014f,  0.184f,  0.576f,
		0.771f,  0.328f,  0.970f,
		0.406f,  0.615f,  0.116f,
		0.676f,  0.977f,  0.133f,
		0.971f,  0.572f,  0.833f,
		0.140f,  0.616f,  0.489f,
		0.997f,  0.513f,  0.064f,
		0.945f,  0.719f,  0.592f,
		0.543f,  0.021f,  0.978f,
		0.279f,  0.317f,  0.505f,
		0.167f,  0.620f,  0.077f,
		0.347f,  0.857f,  0.137f,
		0.055f,  0.953f,  0.042f,
		0.714f,  0.505f,  0.345f,
		0.783f,  0.290f,  0.734f,
		0.722f,  0.645f,  0.174f,
		0.302f,  0.455f,  0.848f,
		0.225f,  0.587f,  0.040f,
		0.517f,  0.713f,  0.338f,
		0.053f,  0.959f,  0.120f,
		0.393f,  0.621f,  0.362f,
		0.673f,  0.211f,  0.457f,
		0.820f,  0.883f,  0.371f,
		0.982f,  0.099f,  0.879f,
		0.583f,  0.771f,  0.014f,
		0.609f,  0.115f,  0.436f,
		0.327f,  0.483f,  0.844f,
		0.822f,  0.569f,  0.201f,
		0.435f,  0.602f,  0.223f,
		0.310f,  0.747f,  0.185f,
		0.597f,  0.770f,  0.761f,
		0.559f,  0.436f,  0.730f,
		0.359f,  0.583f,  0.152f,
		0.483f,  0.596f,  0.789f,
		0.559f,  0.861f,  0.639f,
		0.195f,  0.548f,  0.859f,
		0.014f,  0.184f,  0.576f,
		0.771f,  0.328f,  0.970f,
		0.406f,  0.615f,  0.116f,
		0.676f,  0.977f,  0.133f,
		0.971f,  0.572f,  0.833f,
		0.140f,  0.616f,  0.489f,
		0.997f,  0.513f,  0.064f,
		0.945f,  0.719f,  0.592f,
		0.543f,  0.021f,  0.978f,
		0.279f,  0.317f,  0.505f,
		0.167f,  0.620f,  0.077f,
		0.347f,  0.857f,  0.137f,
		0.055f,  0.953f,  0.042f,
		0.714f,  0.505f,  0.345f,
		0.783f,  0.290f,  0.734f,
		0.722f,  0.645f,  0.174f,
		0.302f,  0.455f,  0.848f,
		0.225f,  0.587f,  0.040f,
		0.517f,  0.713f,  0.338f,
		0.053f,  0.959f,  0.120f,
		0.393f,  0.621f,  0.362f,
		0.673f,  0.211f,  0.457f,
		0.820f,  0.883f,  0.371f,
		0.982f,  0.099f,  0.879f
	};


	GLuint MatrixID = glGetUniformLocation(textureShader, "MVP");
	GLuint DiffuseMVP = glGetUniformLocation(diffuseShader, "MVP");
	GLuint DiffuseM = glGetUniformLocation(diffuseShader, "M");
	GLuint DiffuseV = glGetUniformLocation(diffuseShader, "V");
	GLuint LightID = glGetUniformLocation(diffuseShader, "LightPosition_worldspace");

	
	loadMap("test");

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	//GLuint Texture = loadDDS("textures/uvtemplate.dds");
	/*GLuint CharTexture = loadDDS("textures/charmander.dds");
	GLuint GrassTexture = loadDDS("textures/grass.dds");
	GLuint DragonTexture = loadDDS("textures/dragon.dds");*/
	if (!LoadAllShaders) {
		cout << "[engine.cpp] Some shaders failed to load! Expect graphic problems" << endl;
	}
	if (!loadAllTextures()) {
		printf("[engine.cpp] Some textures failed to load!\n");
	}
	GLuint TextureID = glGetUniformLocation(textureShader, "myTextureSampler");
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	loadOBJ("models/charmander.obj", vertices, uvs, normals);

	GLuint vertexbufferChar;
	glGenBuffers(1, &vertexbufferChar);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferChar);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint uvbufferChar;
	glGenBuffers(1, &uvbufferChar);
	glBindBuffer(GL_ARRAY_BUFFER, uvbufferChar);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	GLuint normalbufferChar;
	glGenBuffers(1, &normalbufferChar);
	glBindBuffer(GL_ARRAY_BUFFER, normalbufferChar);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	std::vector<glm::vec3> vertices2;
	std::vector<glm::vec2> uvs2;
	std::vector<glm::vec3> normals2;
	loadOBJ("models/cube.obj", vertices2, uvs2, normals2);

	GLuint vertexbufferCube;
	glGenBuffers(1, &vertexbufferCube);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferCube);
	glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(glm::vec3), &vertices2[0], GL_STATIC_DRAW);

	GLuint uvbufferCube;
	glGenBuffers(1, &uvbufferCube);
	glBindBuffer(GL_ARRAY_BUFFER, uvbufferCube);
	glBufferData(GL_ARRAY_BUFFER, uvs2.size() * sizeof(glm::vec2), &uvs2[0], GL_STATIC_DRAW);

	std::vector<glm::vec3> vertices3;
	std::vector<glm::vec2> uvs3;
	std::vector<glm::vec3> normals3;
	loadOBJ("models/dragon.obj", vertices3, uvs3, normals3);

	GLuint vertexbufferDragon;
	glGenBuffers(1, &vertexbufferDragon);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferDragon);
	glBufferData(GL_ARRAY_BUFFER, vertices3.size() * sizeof(glm::vec3), &vertices3[0], GL_STATIC_DRAW);

	GLuint uvbufferDragon;
	glGenBuffers(1, &uvbufferDragon);
	glBindBuffer(GL_ARRAY_BUFFER, uvbufferDragon);
	glBufferData(GL_ARRAY_BUFFER, uvs3.size() * sizeof(glm::vec2), &uvs3[0], GL_STATIC_DRAW);

	GLuint normalbufferDragon;
	glGenBuffers(1, &normalbufferDragon);
	glBindBuffer(GL_ARRAY_BUFFER, normalbufferDragon);
	glBufferData(GL_ARRAY_BUFFER, normals3.size() * sizeof(glm::vec3), &normals3[0], GL_STATIC_DRAW);

	std::vector<glm::vec3> vertices4;
	std::vector<glm::vec2> uvs4;
	std::vector<glm::vec3> normals4;
	loadOBJ("models/ground.obj", vertices4, uvs4, normals4);

	GLuint vertexbufferGround;
	glGenBuffers(1, &vertexbufferGround);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferGround);
	glBufferData(GL_ARRAY_BUFFER, vertices4.size() * sizeof(glm::vec3), &vertices4[0], GL_STATIC_DRAW);

	GLuint uvbufferGround;
	glGenBuffers(1, &uvbufferGround);
	glBindBuffer(GL_ARRAY_BUFFER, uvbufferGround);
	glBufferData(GL_ARRAY_BUFFER, uvs4.size() * sizeof(glm::vec2), &uvs4[0], GL_STATIC_DRAW);

	//Enable Face Culling, so the insides of meshes won't be rendered
	glEnable(GL_CULL_FACE);
	//Enable Z-index depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	double lastTime = glfwGetTime();
	int nbFrames = 0;
	double msPerFrame = 0;
	int loading = 100;

	while (!glfwWindowShouldClose(window)) {
		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
			// printf and reset timer
			//printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			msPerFrame = 1000.0 / double(nbFrames);
			nbFrames = 0;
			lastTime += 1.0;
		}

		float ratio;
		int width, height;

		/*i++;
		if (i >= 25) {
			static GLfloat g_color_buffer_data[12 * 3 * 3];
			for (int v = 0; v < 12 * 3; v++) {
				g_color_buffer_data[3 * v + 0] = rand() / (float)RAND_MAX;
				g_color_buffer_data[3 * v + 1] = rand() / (float)RAND_MAX;
				g_color_buffer_data[3 * v + 2] = rand() / (float)RAND_MAX;
			}
			glGenBuffers(1, &colorbuffer);
			glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

			i = 0;
		}*/

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*if (loading > 0) {
			glClearColor(0, 0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT);
			printText2D("Loading...", 290, 290, 20);
			glfwSwapBuffers(window);
			glfwPollEvents();
			loading--;
			continue;
		}*/

		computeMatricesFromInputs(window);
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();

		glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.3f, 0));
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		//RenderTexturedModel(textureShader, MVP, vertexbufferChar, uvbufferChar, vertices.size(), MatrixID);
		//RenderTexturedModel(textureShader, MVP, vertexbufferCube, uvbufferCube, vertices2.size(), MatrixID);



		// Use our shader
		glUseProgram(diffuseShader);
		glUniformMatrix4fv(DiffuseMVP, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(DiffuseM, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(DiffuseV, 1, GL_FALSE, &ViewMatrix[0][0]);
		glm::vec3 lightPos = glm::vec3(4, 4, 4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureMap["charmander.DDS"]);
		glUniform1i(TextureID, 0);

		// 1st attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbufferChar);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbufferChar);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		//3rd attribute, normals, used for shading
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbufferChar);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, vertices.size()); // 3 indices starting at 0 -> 1 triangle

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		glm::mat4 MVP2 = ProjectionMatrix * ViewMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(3, -1, 0));

		// Use our shader
		glUseProgram(textureShader);
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP2[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureMap["grass.DDS"]);
		glUniform1i(TextureID, 0);

		// 1st attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbufferCube);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbufferCube);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, vertices2.size()); // 3 indices starting at 0 -> 1 triangle

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glm::mat4 ModelMatrix3 = glm::translate(glm::mat4(1.0f), glm::vec3(-5, -2, 0));
		glm::mat4 MVP3 = ProjectionMatrix * ViewMatrix * ModelMatrix3;

		// Use our shader
		glUseProgram(diffuseShader);
		glUniformMatrix4fv(DiffuseMVP, 1, GL_FALSE, &MVP3[0][0]);
		glUniformMatrix4fv(DiffuseM, 1, GL_FALSE, &ModelMatrix3[0][0]);
		glUniformMatrix4fv(DiffuseV, 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureMap["dragon.DDS"]);
		glUniform1i(TextureID, 0);

		// 1st attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbufferDragon);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbufferDragon);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbufferDragon);
		glVertexAttribPointer(
			2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, vertices3.size()); // 3 indices starting at 0 -> 1 triangle

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		glm::mat4 MVP4 = ProjectionMatrix * ViewMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(0, -2, 0));

		// Use our shader
		glUseProgram(textureShader);
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP4[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureMap["grass.DDS"]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glUniform1i(TextureID, 0);

		// 1st attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbufferGround);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbufferGround);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, vertices4.size()); // 3 indices starting at 0 -> 1 triangle

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glm::mat4 MVP5 = ProjectionMatrix * ViewMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(5, 0, 0));

		// Use our shader
		glUseProgram(programID);
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP5[0][0]);

		// 1st attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbufferCube);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, vertices2.size()); // 3 indices starting at 0 -> 1 triangle

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		//printText2D("Hello World!", 10, 10, 50);
		printText2D(string(to_string(msPerFrame) + " ms/frame").c_str(), 10, 580, 10);

		//glm::mat4 MVP2 = ProjectionMatrix * ViewMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(3,0,0));

		//RenderModel(programID, MVP2, vertexbufferCube, colorbuffer, vertices2.size());

		/*glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE, GrassTexture);
		glUniform1i(TextureID, 0);

		glm::mat4 MVP2 = ProjectionMatrix * ViewMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(3,0,0));

		RenderTexturedModel(textureShader, MVP2, vertexbufferCube, uvbufferCube, 176 * 2);*/

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cleanupText2D();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
