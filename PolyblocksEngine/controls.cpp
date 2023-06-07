#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include "aabb.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix() {
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix() {
	return ProjectionMatrix;
}

// position
glm::vec3 position = glm::vec3(0, 0, 5);
glm::vec3 velocity = glm::vec3(0, 0, 0);
// horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// vertical angle : 0, look at the horizon
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

void setPlayerPosition(glm::vec3 pos) {
	position = pos;
}

float defaultSpeed = 3.0f;
float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.05f;
float deccelerationSpeed = 0.5f;

//AABB playerAABB = AABB(position.x, position.y, position.z, 0.5f, 1.0f, 0.5f);

float oldXpos = 0;
float oldYpos = 0;
bool noclip = false;

void computeMatricesFromInputsFreeCam(GLFWwindow* window) {

	static double lastTime = glfwGetTime();
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	int windowW, windowH;
	glfwGetWindowSize(window, &windowW, &windowH);

	glfwSetCursorPos(window, windowW / 2, windowH / 2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * deltaTime * float(windowW / 2 - xpos);
	verticalAngle += mouseSpeed * deltaTime * float(windowH / 2 - ypos);

	//Lock vertical angle so you can't look upside down
	if (verticalAngle > 1.6f) {
		verticalAngle = 1.6f;
	}
	else if (verticalAngle < -1.6f) {
		verticalAngle = -1.6f;
	}

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);

	//#############POSITION##############
	// Up vector : perpendicular to both direction and right

	glm::vec3 up = glm::cross(right, direction);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		speed = 16.0f;
	}
	else {
		speed = defaultSpeed;
	}

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= right * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		position += up * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		position -= up * deltaTime * speed;
	}

	float FoV = initialFoV;

	// Projection matrix : 45&deg; Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), (float)windowW / windowH, 0.1f, 1000.0f);
	// Camera matrix
	ViewMatrix = glm::lookAt(
		position,           // Camera is here
		position + direction, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
	);

	lastTime = currentTime;
}

void computeMatricesFromInputsGravity(GLFWwindow* window) {

	static double lastTime = glfwGetTime();
	static bool onGround = true;
	static float yAcc = 0;
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Slow down the velocity using decceleration
	velocity.x -= velocity.x * deccelerationSpeed * deltaTime;
	velocity.z -= velocity.z * deccelerationSpeed * deltaTime;

	velocity.y -= 9.81f * deltaTime;

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	int windowW, windowH;
	glfwGetWindowSize(window, &windowW, &windowH);

	glfwSetCursorPos(window, windowW / 2, windowH / 2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * deltaTime * float(windowW / 2 - xpos);
	verticalAngle += mouseSpeed * deltaTime * float(windowH / 2 - ypos);

	//Lock vertical angle so you can't look upside down
	if (verticalAngle > 1.6f) {
		verticalAngle = 1.6f;
	}
	else if (verticalAngle < -1.6f) {
		verticalAngle = -1.6f;
	}

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	glm::vec3 forward(
		sin(horizontalAngle),
		0,
		cos(horizontalAngle)
	);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);

	//#############POSITION##############
	// Up vector : perpendicular to both direction and right

	glm::vec3 up = glm::cross(right, direction);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		speed = 16.0f;
	}
	else {
		speed = defaultSpeed;
	}

	if (yAcc >= 20) {
		yAcc = 20;
	}
	if (position.y <= 0) {
		onGround = true;
		velocity.y = 0;
	}
	else {
		onGround = false;
	}

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		velocity += forward * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		velocity -= forward * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		velocity += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		velocity -= right * deltaTime * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && onGround) {
		velocity.y = 5/60;
	}

	position += velocity * deltaTime;

	//playerAABB.updatePosition(position.x, position.y, position.z);

	float FoV = initialFoV;

	// Projection matrix : 45&deg; Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), (float)windowW / windowH, 0.1f, 1000.0f);
	glm::vec3 finalPos = position + glm::vec3(0, 1.7f, 0);
	// Camera matrix
	ViewMatrix = glm::lookAt(
		finalPos,           // Camera is here
		finalPos + direction, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
	);

	lastTime = currentTime;
}

void computeMatricesFromInputs(GLFWwindow* window) {
	if (noclip) {
		computeMatricesFromInputsFreeCam(window);
	}
	else {
		computeMatricesFromInputsGravity(window);
	}
}