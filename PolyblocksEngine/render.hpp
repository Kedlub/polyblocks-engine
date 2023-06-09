#ifndef RENDER_HPP
#define RENDER_HPP

extern std::vector<std::string> textBuffer;

void initText2D(const char *texturePath, const char *outlineTexturePath, const char *boldTexturePath,
                const char *boldOutlineTexturePath);

void printText2D(const char *text, int x, int y, int size, bool outlined = false, bool bold = false);

void cleanupText2D();

void RenderModel(GLuint programID, glm::mat4 mvp, GLuint vertexbuffer, GLuint colorbuffer, int triNumber);

void RenderTexturedModel(GLuint programID, glm::mat4 mvp, GLuint vertexbuffer, GLuint uvbuffer, int triNumber,
                         GLuint MatrixID);

#endif // !RENDER_HPP
