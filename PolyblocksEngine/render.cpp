#include <glad/glad.h>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <cstring>
#include "shader.hpp"
#include "texture.hpp"
#include "utils.hpp"

std::vector<std::string> textBuffer;
std::vector<glm::vec2> vertices;
std::vector<glm::vec2> UVs;

unsigned int Text2DTextureID;
unsigned int Text2DTextureIDOutline;
unsigned int Text2DTextureIDBold;
unsigned int Text2DTextureIDBoldOutline;
unsigned int Text2DVertexBufferID;
unsigned int Text2DUVBufferID;
unsigned int Text2DShaderID;
unsigned int Text2DUniformID;

void initText2D(const char* texturePath, const char* outlineTexturePath, const char* boldTexturePath, const char* boldOutlineTexturePath) {

	// Initialize texture
	Text2DTextureID = loadDDS(texturePath);
	Text2DTextureIDOutline = loadDDS(outlineTexturePath);
	Text2DTextureIDBold = loadDDS(boldTexturePath);
	Text2DTextureIDBoldOutline = loadDDS(boldOutlineTexturePath);

	// Initialize VBO
	glGenBuffers(1, &Text2DVertexBufferID);
	glGenBuffers(1, &Text2DUVBufferID);

	// Initialize Shader
	Text2DShaderID = LoadShaders("shaders/TextShader.vertexshader", "shaders/TextShader.fragmentshader");

	// Initialize uniforms' IDs
	Text2DUniformID = glGetUniformLocation(Text2DShaderID, "myTextureSampler");

}

void printText2D(const char* text, int x, int y, int size, bool outlined = false, bool bold = false) {

	if (text == "") return;

	unsigned int length = strlen(text);

	// Fill buffers
	std::vector<glm::vec2> vertices;
	std::vector<glm::vec2> UVs;
	for (unsigned int i = 0; i < length; i++) {
		float char_width = (size / 2);
		/*glm::vec2 vertex_up_left = glm::vec2(x + i * (size/1.4f), y + size);
		glm::vec2 vertex_up_right = glm::vec2(x + i * (size / 1.4f) + size, y + size);
		glm::vec2 vertex_down_right = glm::vec2(x + i * (size / 1.4f) + size, y);
		glm::vec2 vertex_down_left = glm::vec2(x + i * (size / 1.4f), y);*/
		/*glm::vec2 vertex_up_left = glm::vec2(x + i * (size/2), y + size);
		glm::vec2 vertex_up_right = glm::vec2(x + i * (size/2) + size/2, y + size);
		glm::vec2 vertex_down_right = glm::vec2(x + i * (size/2) + size/2, y);
		glm::vec2 vertex_down_left = glm::vec2(x + i * (size/2), y);*/
		/*glm::vec2 vertex_up_left = glm::vec2(x + i * (size / 1.25f), y + size);
		glm::vec2 vertex_up_right = glm::vec2(x + i * (size / 1.25f) + size, y + size);
		glm::vec2 vertex_down_right = glm::vec2(x + i * (size / 1.25f) + size, y);
		glm::vec2 vertex_down_left = glm::vec2(x + i * (size / 1.25f), y);*/

		glm::vec2 vertex_up_left = glm::vec2(x + i * char_width, y + size);
		glm::vec2 vertex_up_right = glm::vec2(x + i * char_width + char_width, y + size);
		glm::vec2 vertex_down_right = glm::vec2(x + i * char_width + char_width, y);
		glm::vec2 vertex_down_left = glm::vec2(x + i * char_width, y);

		vertices.push_back(vertex_up_left);
		vertices.push_back(vertex_down_left);
		vertices.push_back(vertex_up_right);

		vertices.push_back(vertex_down_right);
		vertices.push_back(vertex_up_right);
		vertices.push_back(vertex_down_left);

		char character = text[i];
		int characterID = getFixedCharID(character);
		float uv_x = (characterID % 32) / 32.0f;
		float uv_y = (characterID / 32) / 16.0f;
		//printf("character = %s, uv_x = %s, uv_y = %s\n", std::to_string(character).c_str(), std::to_string(uv_x).c_str(), std::to_string(uv_y).c_str());

		glm::vec2 uv_up_left = glm::vec2(uv_x, uv_y);
		glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f / 32.0f, uv_y);
		glm::vec2 uv_down_right = glm::vec2(uv_x + 1.0f / 32.0f, (uv_y + 1.0f / 16.0f));
		glm::vec2 uv_down_left = glm::vec2(uv_x, (uv_y + 1.0f / 16.0f));
		UVs.push_back(uv_up_left);
		UVs.push_back(uv_down_left);
		UVs.push_back(uv_up_right);

		UVs.push_back(uv_down_right);
		UVs.push_back(uv_up_right);
		UVs.push_back(uv_down_left);
	}
	glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);

	// Bind shader
	glUseProgram(Text2DShaderID);

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, outlined && bold ? Text2DTextureIDBoldOutline : (outlined ? Text2DTextureIDOutline : (bold ? Text2DTextureIDBold : Text2DTextureID)));
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(Text2DUniformID, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw call
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

}

void cleanupText2D() {

	// Delete buffers
	glDeleteBuffers(1, &Text2DVertexBufferID);
	glDeleteBuffers(1, &Text2DUVBufferID);

	// Delete texture
	glDeleteTextures(1, &Text2DTextureID);

	// Delete shader
	glDeleteProgram(Text2DShaderID);
}

void RenderModel(GLuint programID, glm::mat4 mvp, GLuint vertexbuffer, GLuint colorbuffer, int triNumber) {
	// Get a handle for our "MVP" uniform
		// Only during the initialisation
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

	// Use our shader
	glUseProgram(programID);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, triNumber); // 3 indices starting at 0 -> 1 triangle

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void RenderTexturedModel(GLuint programID, glm::mat4 mvp, GLuint vertexbuffer, GLuint uvbuffer, int triNumber, GLuint MatrixID) {
	// Get a handle for our "MVP" uniform
		// Only during the initialisation
	

	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

	// Use our shader
	glUseProgram(programID);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
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
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, triNumber); // 3 indices starting at 0 -> 1 triangle

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}