#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include "world.hpp"
#include "utils.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "objloader.hpp"
#include "render.hpp"
#include "gui.hpp"
#include "controls.hpp"
#include "aabb.hpp"
#include <chrono>
#include <thread>

World currentWorld;
aabb::Tree worldTree(3);
bool inGame = false;

//Base implementation of render, other class which has to render something must implement this
void BaseObject::render(glm::mat4 viewMatrix, glm::mat4 projectMatrix) {
	//don't render base object or invisible object, it has nothing to render!
	//printf("BaseObject.render() got called. Something is wrong.\n");
}

//Create a Static Prop, model which will not be moved
StaticProp::StaticProp(Model mdl, std::string tex, glm::vec3 pos, glm::vec3 size)
{
	model = mdl;
	texture = tex;
	position = pos;
	this->size = size;
}

void StaticProp::render(glm::mat4 viewMatrix, glm::mat4 projectMatrix) {
	glm::mat4 sizeMatrix = glm::scale(glm::mat4(1.0f), size);
	glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0f), position) * sizeMatrix;
	glm::mat4 MVP = projectMatrix * viewMatrix * ModelMatrix;

	GLuint shader = shaderMap["Unlit"];
	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "M"), 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "V"), 1, GL_FALSE, &viewMatrix[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureMap[texture]);
	glUniform1i(glGetUniformLocation(shader, "myTextureSampler"), 0);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, model.verticesID);
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
	glBindBuffer(GL_ARRAY_BUFFER, model.uvID);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)model.vertices.size()); // 3 indices starting at 0 -> 1 triangle

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

//Create Brush, a basic world object (wall, floor)
Brush::Brush(glm::vec3 f, glm::vec3 t, std::string tex, int aabb)
{
	from = f;
	to = t;
	texture = tex;
	// Calculate center position and extents for AABB
	glm::vec3 center = (from + to) / 2.0f;
	glm::vec3 extents = (to - from) / 2.0f;
	//aabb = AABB(center.x, center.y, center.z, extents.x, extents.y, extents.z);
	aabbID = aabb;
}

void Brush::GenBuffer() {
	//printf("[Model Loader] Generating buffers for brush with texture %s\n", std::string(texture));
	std::cout << "[Model Loader] Generating buffer for brush with texture " << texture << std::endl;
	/*std::vector<GLfloat> vertices = {
		-1,1,-1,
		1,1,1,
		1,1,-1,
		1,1,1,
		-1,-1,1,
		1,-1,1,
		-1,1,1,
		-1,-1,-1,
		-1,-1,1,
		1,-1,-1,
		-1,-1,1,
		-1,-1,-1,
		1,1,-1,
		1,-1,1,
		1,-1,-1,
		-1,1,-1,
		1,-1,-1,
		-1,-1,-1,
		-1,1,-1,
		-1,1,1,
		1,1,1,
		1,1,1,
		-1,1,1,
		-1,-1,1,
		-1,1,1,
		-1,1,-1,
		-1,-1,-1,
		1,-1,-1,
		1,-1,1,
		-1,-1,1,
		1,1,-1,
		1,1,1,
		1,-1,1,
		-1,1,-1,
		1,1,-1
	};*/
	const std::vector<glm::vec3>& vertices = loadOBJModel("cube.obj").vertices;
	float xsize = from.x - to.x;
	float ysize = from.y - to.y;
	float zsize = from.z - to.z;
	
	std::vector<GLfloat> uvs = {
		0,-zsize,
		xsize,0,
		xsize,-zsize,

		xsize, -ysize,
		0, 0,
		xsize, 0,

		0,-ysize,
		zsize,0,
		0,0,

		xsize,-zsize,
		0,0,
		0,-zsize,

		zsize,-ysize,
		0,0,
		zsize,0,

		0,-ysize,
		xsize,0,
		0,0,

		0,-zsize,
		0,0,
		xsize,0,

		xsize,-ysize,
		0,-ysize,
		0,0,

		0,-ysize,
		zsize,-ysize,
		zsize,-0,

		xsize,-zsize,
		xsize,0,
		0,0,

		zsize,-ysize,
		0,-ysize,
		0,0,

		0,-ysize,
		xsize,-ysize,
		xsize,0
	};
 	glGenBuffers(1, &verticesID);
	glBindBuffer(GL_ARRAY_BUFFER, verticesID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvID);
	glBindBuffer(GL_ARRAY_BUFFER, uvID);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	vertexCount = (int)vertices.size();
}

//Render brush with the info it contains
void Brush::render(glm::mat4 viewMatrix, glm::mat4 projectMatrix) {
	//old debug and broken code
	//printf("Rendering brush\n");
	//glm::vec3 position = glm::vec3(((from.x + 0.5f) + (to.x - 0.5f) / 2), ((from.y + 0.5f) + (to.y - 0.5f) / 2), ((from.z + 0.5f) + (to.z - 0.5f) / 2));
	//glm::vec3 size = glm::vec3(glm::distance(from.x + 0.5f, to.x - 0.5f) / 2, glm::distance(from.y + 0.5f, to.y - 0.5f) / 2, glm::distance(from.z + 0.5f, to.z - 0.5f) / 2);

	//Hynek Fisera helped with position calculation
	glm::vec3 position = glm::vec3((from.x + to.x) / 2, (from.y + to.y) / 2, (from.z + to.z) / 2);
	glm::vec3 size = glm::vec3(glm::distance(from.x, to.x)/2, glm::distance(from.y, to.y)/2, glm::distance(from.z, to.z)/2);
	glm::mat4 SizeMatrix = glm::scale(glm::mat4(1.0f), size);
	glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0f), position) * SizeMatrix;
	//std::cout << "Rendering model at x:" << position.x << " y:" << position.y << " z:" << position.z << " With size x:" << size.x << " y:" << size.y << " z:" << size.z << std::endl;
	/*std::stringstream buffer;
	buffer << "Rendering model at x:" << position.x << " y:" << position.y << " z:" << position.z << " With size x:" << size.x << " y:" << size.y << " z:" << size.z << " distance between from x and to x is: " << glm::distance(from.x, to.x) / 2 << std::endl;
	textBuffer.push_back(buffer.str());*/
	//glm::mat4 ModelMatrix = glm::mat4(1.0f);
	glm::mat4 MVP = projectMatrix * viewMatrix * ModelMatrix;
	GLuint shader = shaderMap["Unlit"];
	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "M"), 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "V"), 1, GL_FALSE, &viewMatrix[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureMap[texture]);
	glUniform1i(glGetUniformLocation(shader, "myTextureSampler"), 0);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, verticesID);
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
	glBindBuffer(GL_ARRAY_BUFFER, uvID);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, vertexCount); // 3 indices starting at 0 -> 1 triangle

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}


enum class MapType
{
	Normal, Voxel
};

//Loads map from maps folder, and returns World object with it
World loadMap(const char* mapName) {
	std::ifstream input("maps/" + std::string(mapName) + ".bmap");
	printf("[Map Loader] Loading map \"%s\"\n", mapName);
	drawInfo("Loading map...", mapName);
	noclip = true;
	setPlayerPosition(glm::vec3());
	int lineID = 1;
	MapType type;
	std::vector<BaseObject*> objects;
	std::vector<std::string> texNames;
	int index = 1;
	for (std::string line; getline(input, line); )
	{
		//printf("[Map Loader] Line %s: %s\n", std::string(std::to_string(lineID)).c_str(), std::string(line).c_str());
		if (line.rfind("#", 0) == 0) continue;
		if (lineID == 1 && line.find("type") == std::string::npos) {
			printf("[Map Loader] ERROR! Map files must begin with TYPE function\n");
			return World("Map files must begin with TYPE function!");
		}
		if (line.find("type") != std::string::npos) {
			if (line.find("NORMAL") != std::string::npos) {
				printf("[Map Loader] type = NORMAL\n");
				type = MapType::Normal;
			}
			else if (line.find("VOXEL") != std::string::npos) {
				printf("[Map Loader] ERROR! Voxel map type not supported yet!\n");
				return World("Voxel map not supported yet!");
			}
		}
		if (line.find("staticprop") != std::string::npos) {
			std::vector<std::string> str;
			glm::vec3 pos = glm::vec3();
			glm::vec3 size = glm::vec3();
			std::string modelName;
			std::string texture;
			split(line, str, ' ');
			for (size_t i = 0; i < str.size(); i++)
			{
				std::string var = str[i];
				if (i == 1) {
					var.erase(std::remove(var.begin(), var.end(), '['), var.end());
					var.erase(std::remove(var.begin(), var.end(), ']'), var.end());
					std::vector<std::string> posStr;
					split(var, posStr, ',');
					pos = glm::vec3(std::stof(posStr[0]), std::stof(posStr[1]), std::stof(posStr[2]));
				}
				else if (i == 2) {
					var.erase(std::remove(var.begin(), var.end(), '['), var.end());
					var.erase(std::remove(var.begin(), var.end(), ']'), var.end());
					std::vector<std::string> sizeStr;
					split(var, sizeStr, ',');
					size = glm::vec3(std::stof(sizeStr[0]), std::stof(sizeStr[1]), std::stof(sizeStr[2]));
				}
				else if (i == 3) {
					modelName = var;
				}
				else if (i == 4) {
					texture = var;
				}
			}
			Model mdl = loadOBJModel(modelName.c_str());
			StaticProp& prop = *new StaticProp(mdl, texture, pos, size);
			objects.push_back(&prop);
			texNames.push_back(texture);
		}
		if (line.find("brush") != std::string::npos) {
			std::vector<std::string> str;
			split(line, str, ' ');
			int argID = 0;
			std::vector<std::string> pos1;
			std::vector<std::string> pos2;
			std::string tex;
			for (auto var : str)
			{
				//printf("Parsing string %s\n", std::string(var));
				//std::cout << "Parsing string " << var << std::endl;
				if (argID == 1) {
					var.erase(std::remove(var.begin(), var.end(), '['), var.end());
					var.erase(std::remove(var.begin(), var.end(), ']'), var.end());
					//std::cout << "Removed brackets from " << var << std::endl;
					split(var, pos1, ',');
				}
				if (argID == 2) {
					var.erase(std::remove(var.begin(), var.end(), '['), var.end());
					var.erase(std::remove(var.begin(), var.end(), ']'), var.end());
					//std::cout << "Removed brackets from " << var << std::endl;
					split(var, pos2, ',');
				}
				if (argID == 3) {
					tex = var;
				}
				argID++;
			}
			//std::cout << "[Map Loader] Creating brush , original string, from: [" << pos1[0] << "," << pos1[1] << "," << pos1[2] << "] to: [" << pos2[0] << "," << pos2[1] << "," << pos1[2] << "]\n";
			//std::cout << "[Map Loader] Creating brush, from: [" << std::stof(pos1[0]) << "," << std::stof(pos1[1]) << "," << std::stof(pos1[2]) << "] to: [" << std::stof(pos2[0]) << "," << std::stof(pos2[1]) << "," << std::stof(pos1[2]) << "]\n";

			// Get the bounds of the brush (the two corners)
			// Pos1 and Pos2 contains the two corners of the brush, but they are not sorted, so we need to calculate the bounds
			// First we calculate the center position
			std::vector<double> centerPosition = { (std::stof(pos1[0]) + std::stof(pos2[0])) / 2.0f, (std::stof(pos1[1]) + std::stof(pos2[1])) / 2.0f, (std::stof(pos1[2]) + std::stof(pos2[2])) / 2.0f };
			// Then we calculate the bounds

			std::vector<double> lowerBound = { centerPosition[0] - std::abs(std::stof(pos1[0]) - std::stof(pos2[0])) / 2.0f, centerPosition[1] - std::abs(std::stof(pos1[1]) - std::stof(pos2[1])) / 2.0f, centerPosition[2] - std::abs(std::stof(pos1[2]) - std::stof(pos2[2])) / 2.0f };
			std::vector<double> upperBound = { centerPosition[0] + std::abs(std::stof(pos1[0]) - std::stof(pos2[0])) / 2.0f, centerPosition[1] + std::abs(std::stof(pos1[1]) - std::stof(pos2[1])) / 2.0f, centerPosition[2] + std::abs(std::stof(pos1[2]) - std::stof(pos2[2])) / 2.0f };

			//std::vector<double> lowerBound({ std::stof(pos1[0]), std::stof(pos1[1]), std::stof(pos1[2])});
			//std::vector<double> upperBound({ std::stof(pos2[0]), std::stof(pos2[1]), std::stof(pos2[2])});
			
			// Print out the bounds
			std::cout << "Lower bound: ";
			for (auto i : lowerBound) {
			  std::cout << i << " ";
			}
			std::cout << std::endl;

			std::cout << "Upper bound: ";
			for (auto i : upperBound) {
			  std::cout << i << " ";
			}
			std::cout << std::endl;

			if(isLowerBoundGreaterThanUpper(lowerBound, upperBound) == true) {
			  std::swap(lowerBound, upperBound);
			}

			worldTree.insertParticle(index, lowerBound, upperBound);
			index++;
			Brush& br = *new Brush(glm::vec3(std::stof(pos1[0]), std::stof(pos1[1]), std::stof(pos1[2])), glm::vec3(std::stof(pos2[0]), std::stof(pos2[1]), std::stof(pos2[2])), tex, index);
			br.GenBuffer();
			objects.push_back(&br);
			texNames.push_back(tex);
		}
		if (line.find("spawn") != std::string::npos) {
			noclip = false;
			std::vector<std::string> str;
			std::vector<std::string> strPos;
			split(line, str, ' ');
			for (int i = 0; i < str.size(); i++) {
				std::string var = str[i];
				if (i == 1) {
					var.erase(std::remove(var.begin(), var.end(), '['), var.end());
					var.erase(std::remove(var.begin(), var.end(), ']'), var.end());
					//std::cout << "Removed brackets from " << var << std::endl;
					split(var, strPos, ',');
				}
			}
			setPlayerPosition(glm::vec3(std::stoi(strPos[0]), std::stoi(strPos[1]), std::stoi(strPos[2])));
		}
		lineID++;
	}
	if (!loadAllTextures(texNames)) {
		printf("[Map Loader] Failed to load some textures!\n");
	}
	std::this_thread::sleep_for(std::chrono::seconds(1));
	printf("[Map Loader] Finished loading map %s\n", mapName);
	World world = World(objects);
	//currentWorld = world;
	return world;
}

World::World()
{
	isNull = true;
}

World::World(std::vector<BaseObject*> staticObjects) : staticObjects(staticObjects)
{
	isNull = false;
}

World::World(std::string errorText) : errorText(errorText)
{
	isNull = true;
}

void World::render(glm::mat4 viewMatrix, glm::mat4 projectMatrix)
{
	for(auto obj : staticObjects)
	{
		obj->render(viewMatrix, projectMatrix);
	}
}

int World::getObjectsSize()
{
	return (int)staticObjects.size();
}

World::~World()
{

}
