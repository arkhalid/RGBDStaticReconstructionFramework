#include "controls.h"

void Controls::init(const int winX, const int winY)
{
	winWidth = winX;
	winHeight = winY;

	// Initial horizontal angle : toward -Z
	horizontalAngle = 3.14f;
	// Initial vertical angle : none
	verticalAngle = 0.0f;

	walkThroughSpeedScale = 0.1; //10% of the scale of mesh
	meshSize = 1000;
	mouseSpeed = 0.005f;

	mousePosX = winWidth / 2;
	mousePosY = winHeight / 2;

	rotation = vec3(0.0f);
}

void Controls::cameraViewComputation(GLFWwindow* window)
{
	

	// Get mouse position 
	int mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	
	double xpos, ypos ;
	glfwGetCursorPos(window, &xpos, &ypos);

	if (mouseState == GLFW_PRESS)
	{
		// Compute new orientation if mouse is pressed
		horizontalAngle += mouseSpeed * float(mousePosX - xpos);
		verticalAngle += mouseSpeed * float(mousePosY - ypos);
	}
	
	mousePosX = xpos;
	mousePosY = ypos;

}




void Controls::computeMatricesFromInputs(GLFWwindow* window)
{
	
	cameraViewComputation(window);

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	deltaTime = float(currentTime - lastTime);

	//Setting up the walkthrough speed relative to the size of mesh
	walkThroughSpeed = meshSize * walkThroughSpeedScale;

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	direction = vec3(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
		);

	// Right vector
	right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
		);

	// Up vector
	up = glm::cross(right, direction);

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		position += direction * deltaTime * walkThroughSpeed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		position -= direction * deltaTime * walkThroughSpeed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
		position += right * deltaTime * walkThroughSpeed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
		position -= right * deltaTime * walkThroughSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS){
		position += up * deltaTime * walkThroughSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS){
		position -= up * deltaTime * walkThroughSpeed;
	}

	
	

	// Camera matrix
	viewMatrix = glm::lookAt(
		position,           // Camera is here
		position + direction, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
		);
	/*printf("Xpos is %lf\n YPos is %lf\n",xpos,ypos);
	printf("VerticalAngle is %f\n HorizontalAngle is %f\n",verticalAngle,horizontalAngle);
	printf("Direction Vec is %f , %f , %f\n",direction.x , direction.y ,direction.z);*/
	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}
void Controls::scroll_callback(GLFWwindow* window, float drag)
{
	position +=  direction * drag * deltaTime * walkThroughSpeed * 100.0f;
}
