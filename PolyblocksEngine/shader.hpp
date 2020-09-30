#ifndef SHADER_HPP
#define SHADER_HPP
#include <unordered_map>
extern std::unordered_map<std::string, GLuint> shaderMap;


class Shader
{
public:
	std::string shaderName;
	std::string fragmentFile;
	std::string vertexFile;
	Shader(std::string name, std::string ff, std::string vf);

private:

};


bool loadAllShaders();

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

#endif