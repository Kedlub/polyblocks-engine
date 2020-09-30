#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
class Model
{
public:
	std::vector<glm::vec3> vertices;
	GLuint vertexbuffer;
	GLuint uvbuffer;
	Model();
	~Model();
private:

};

Model::Model()
{
}

Model::~Model()
{
}