#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>
#include <unordered_map>

#include <glm/glm.hpp>

#include "objloader.hpp"
#include "utils.hpp"

Model::Model()
{
	verticesID = 0;
	uvID = 0;
	normalsID = 0;
}

void Model::GenBuffers()
{
	if (verticesID != 0 && uvID != 0 && normalsID != 0) {
		printf("[Model Loader] Buffers for model is not 0, skipping gen!\n");
		return;
	}
	printf("[Model Loader] Generating buffers for Model\n");
	glGenBuffers(1, &verticesID);
	glBindBuffer(GL_ARRAY_BUFFER, verticesID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvID);
	glBindBuffer(GL_ARRAY_BUFFER, uvID);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalsID);
	glBindBuffer(GL_ARRAY_BUFFER, normalsID);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
}

Model::~Model()
{
}

std::string modelFiles[] = {
	"cube.obj"
};

std::unordered_map<std::string, Model> modelMap;

bool loadAllModels() {
	bool result = true;
	for (auto var : modelFiles)
	{
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;
		//loadOBJ(std::string("models/").append(var).c_str(), vertices, uvs, normals);
		bool ok = loadOBJ(var.c_str(), vertices, uvs, normals);
		if (!ok) {
			result = false;
		}
	}
	return result;
}

//Usage: loadOBJModel("charmander.obj");
//Use this method to get model with generated buffers
Model loadOBJModel(const char* path) {
	if (modelMap.find(path) != modelMap.end()) {
		printf("[Model Loader] Object %s exists in modelMap, getting it from there\n", path);
		Model mdl = modelMap[path];
		mdl.GenBuffers();
		return mdl;
	}
	printf("[Model Loader] Object %s does not exist in modelMap, loading it now!\n", path);
	std::vector<glm::vec3> vertices;	
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool ok = loadOBJ(path, vertices, uvs, normals);
	Model mdl = Model();
	mdl.vertices = vertices;
	mdl.uvs = uvs;
	mdl.normals = normals;
	mdl.GenBuffers();
	return mdl;
}

// Very, VERY simple OBJ loader.
// Here is a short list of features a real function would provide : 
// - Binary files. Reading a model should be just a few memcpy's away, not parsing a file at runtime. In short : OBJ is not very great.
// - Animations & bones (includes bones weights)
// - Multiple UVs
// - All attributes should be optional, not "forced"
// - More stable. Change a line in the OBJ file and it crashes.
// - More secure. Change another line and you can inject code.
// - Loading from memory, stream, etc
bool loadOBJ(
	const char* path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
) {
	std::string editedPath = std::string("models/") + path;
	//eraseSubStr(editedPath, "models/");
	if (modelMap.find(path) != modelMap.end()) {
		Model mdl = modelMap[path];
		out_vertices = mdl.vertices;
		out_uvs = mdl.uvs;
		out_normals = mdl.normals;
		return true;
	}
	printf("[Model Loader] Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE* file = fopen(editedPath.c_str(), "r");
	if (file == NULL) {
		std::cout << "[Model Loader] Failed to open file " << path << ". Does it exist?\n";
		//getchar();
		return false;
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("[Model Loader] File can't be read by our simple parser :-( Try exporting with other options\n");
				fclose(file);
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];



		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}
	Model mdl = Model();
	mdl.vertices = out_vertices;
	mdl.uvs = out_uvs;
	mdl.normals = out_normals;
	modelMap[path] = mdl;

	std::cout << "[Model Loader] " << path << " vertices count: " << out_vertices.size() << std::endl;
	fclose(file);
	return true;
}