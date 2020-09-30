#ifndef CONTROLS_HPP
#define CONTROLS_HPP
#include <GLFW/glfw3.h>
extern bool noclip;

void setPlayerPosition(glm::vec3 pos);
void computeMatricesFromInputs(GLFWwindow* window);
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

#endif