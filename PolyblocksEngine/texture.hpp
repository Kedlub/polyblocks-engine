#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <unordered_map>
#include <vector>

extern std::unordered_map<std::string, GLuint> textureMap;

bool loadAllTextures(std::vector<std::string> textureFiles);

GLuint loadBMP_custom(const char *imagepath);

GLuint loadDDS(const char *imagepath);

#endif