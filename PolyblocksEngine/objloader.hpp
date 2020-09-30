#ifndef OBJLOADER_H
#define OBJLOADER_H
#include <glad/glad.h>
#include <unordered_map>



class Model
{
public:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	GLuint verticesID;
	GLuint uvID;
	GLuint normalsID;

	Model();
	void GenBuffers();
	~Model();

private:

};

extern std::unordered_map<std::string, Model> modelMap;


Model loadOBJModel(const char* path);
bool loadAllModels();
bool loadOBJ(
	const char* path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
);

#endif