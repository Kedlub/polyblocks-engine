#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include "texture.hpp"

std::unordered_map<std::string, GLuint> textureMap;

/*std::string textureFiles[] = {
	"charmander.DDS",
	"dragon.DDS",
	"grass.DDS",
	"uvtemplate.DDS",
	"brick.DDS",
	"sky_day.DDS"
};*/


bool loadAllTextures(std::vector<std::string> textureFiles) {
	bool ok = true;
	for (std::string& texName : textureFiles)
	{
		if (textureMap.count(texName) >= 1) continue;
		GLuint textureID = loadDDS(std::string("textures/").append(texName).c_str());
		textureMap[texName] = textureID;
		if (textureID == 0) {
			ok = false;
		}
	}

	return ok;
}

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

GLuint loadDDS(const char* imagepath) {

	unsigned char header[124];

	FILE* fp;

	printf("[Texture Loader] Loading %s\n", imagepath);

	/* try to open the file */
	fp = fopen(imagepath, "rb");
	if (fp == NULL) {
		printf("[Texture Loader] Image %s does not exist!\n", imagepath);
		return 0;
	}

	/* verify the type of file */
	char filecode[4];
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS ", 4) != 0) {
		printf("[Texture Loader] Image %s is in wrong format!\n", imagepath);
		fclose(fp);
		return 0;
	}

	//printf("[Texture Loader] Reading file...\n");
	/* get the surface desc */
	fread(&header, 124, 1, fp);

	unsigned int height = *(unsigned int*) & (header[8]);
	unsigned int width = *(unsigned int*) & (header[12]);
	unsigned int linearSize = *(unsigned int*) & (header[16]);
	unsigned int mipMapCount = *(unsigned int*) & (header[24]);
	unsigned int fourCC = *(unsigned int*) & (header[80]);

	unsigned char* buffer;
	unsigned int bufsize;
	/* how big is it going to be including all mipmaps? */
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
	buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
	fread(buffer, 1, bufsize, fp);
	/* close the file pointer */
	fclose(fp);
	//printf("[Texture Loader] Detecting format for %s\n", imagepath);
	unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
	unsigned int format;
	switch (fourCC)
	{
	case FOURCC_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case FOURCC_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case FOURCC_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		printf("[Texture Loader] Image %s is compressed in wrong format!\n", imagepath);
		free(buffer);
		return 0;
	}
	// Create one OpenGL texture
	GLuint textureID;
	//printf("[Texture Loader] Generating textures for %s\n", imagepath);
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;
	//printf("[Texture Loader] Loading mipmaps for image %s...\n", imagepath);
	/* load the mipmaps */
	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
	{
		//printf("[Texture Loader] Loading mipmap %s for %s...\n", std::to_string(level), imagepath);
		//std::cout << "[Texture Loader] Loading mipmap #" << level << " for " << imagepath << std::endl;
		unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
			0, size, buffer + offset);

		offset += size;
		width /= 2;
		height /= 2;
		//std::cout << "[Texture Loader] Loaded mipmap #" << level << " for " << imagepath << std::endl;
		
		
	}
	free(buffer);

	//printf("[Texture Loader] Image %s loaded!\n", imagepath);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (mipMapCount <= 1) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	return textureID;
}