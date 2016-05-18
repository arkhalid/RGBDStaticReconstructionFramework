#ifndef CONTROLS_H
#define CONTROLS_H

// Include GLEW
#include <glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp> 

using namespace glm;


class Controls
{
	public:
		Controls(){}
		void init(const int winX, const int winY);
		void setPosition(const vec3& pos) { position = pos; }
		void computeMatricesFromInputs(GLFWwindow* window);

		void cameraViewComputation(GLFWwindow* window);

		mat4 getViewMatrix(){ return viewMatrix; }
		mat4 getModelMatrix(){ return modelMatrix; }

		void setMouseSpeed(float speed){ mouseSpeed = speed;} 
		void setMeshSize(float size){ meshSize = size;}
		void setWalkThroughSpeedScale(float scale){ walkThroughSpeedScale = scale;} 

		void scroll_callback(GLFWwindow* window, float drag);

	private:

		mat4 viewMatrix;
		mat4 modelMatrix;


		vec3 rotation;
		vec3 position;


		float horizontalAngle;
		float verticalAngle;
		float deltaTime;

		vec3 direction;
		vec3 right;
		vec3 up;

		float meshSize;
		float walkThroughSpeedScale;
		float walkThroughSpeed;
		float mouseSpeed;
		float trackBallRadius;

		int winWidth;
		int winHeight;

		double mousePosX;
		double mousePosY;

};
#endif