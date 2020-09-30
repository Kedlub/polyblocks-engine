#ifndef WORLD_HPP
#define WORLD_HPP
#include <glad/glad.h>
#include "objloader.hpp"



class BaseObject
{
public:
	virtual void render(glm::mat4 viewMatrix, glm::mat4 projectMatrix);

private:

};

class World
{
public:
	bool isNull;
	std::string errorText;
	World();
	World(std::vector<BaseObject*> staticObjects);
	World(std::string errorText);
	void render(glm::mat4 viewMatrix, glm::mat4 projectMatrix);
	int getObjectsSize();

	~World();

private:
	std::vector<BaseObject*> staticObjects;

};
World loadMap(const char* mapName);


class Brush: public BaseObject
{
public:
	glm::vec3 from;
	glm::vec3 to;
	std::string texture;
	int vertexCount = 9;
	GLuint verticesID;
	GLuint uvID;
	GLuint normalID;

	Brush(glm::vec3 f, glm::vec3 t, std::string tex);

	void GenBuffer();

	void render(glm::mat4 viewMatrix, glm::mat4 projectMatrix);

	~Brush()
	{
	}

private:

};

class StaticProp : public BaseObject
{
public:
	std::string texture;
	glm::vec3 position;
	glm::vec3 size;
	Model model;

	StaticProp(Model mdl, std::string tex, glm::vec3 pos, glm::vec3 size);

	void render(glm::mat4 viewMatrix, glm::mat4 projectMatrix);

private:

};

extern World currentWorld;
extern bool inGame;

#endif // WORLD_HPP
