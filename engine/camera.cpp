#include "bopEngine.h"

LIB_API Camera::Camera()
{
}

LIB_API Camera::~Camera()
{
}

/*
Eye : position of the camera
Center : the point where the camera is looking at
Up: Sky orientation
*/
void Camera::setViewMatrix(glm::vec3 eye, glm::vec3 center, glm::vec3 up)
{
	this->viewMatrix = glm::lookAt(eye, center, up);
}

void Camera::setViewMatrix(glm::mat4 view)
{
	this->viewMatrix = view;
}