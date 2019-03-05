#include "bopEngine.h"

// FreeGLUT:   
#include <GL/freeglut.h>

int Light::numberLights{ 0 };

LIB_API Light::Light()
{
	numberLight = numberLights;
	numberLights++;
}
LIB_API Light::~Light()
{
	numberLights--;

}

void LIB_API Light::turnOff()
{
	glDisable(GL_LIGHT0 + numberLight);
}

void LIB_API Light::setLightType(LIGHT_TYPE light_type)
{
	this->light_type = light_type;
	if (light_type == OMNI) {
		//cutoff should always be 180.0f when light_type is OMNI
		this->cutoff = 180.0f;
	}
	else if (light_type == SPOT) {
		//Default value for spot cutoff is 15.0f, can be changed with setCutoff
		this->cutoff = 15.0f;
		//Default value for direction is 1.0f, 1.0f, 1.0f, can be changed with setDirection
		this->direction = glm::vec3(1.0f, 1.0f, 1.0f);
	}
}

void LIB_API Light::setDirection(glm::vec3 direction)
{
	this->direction = direction;
}


void LIB_API Light::setCutoff(float cutoff) {
	if (light_type == SPOT)
		this->cutoff = cutoff;
}



void Light::render(glm::mat4 view)
{
	glm::vec4 position = this->finalPositionMatrix[3];
	
	if (light_type == DIRECTIONAL)
		position[3] = 0.0f;
	else
		position[3] = 1.0f;

	glEnable(GL_LIGHT0 + numberLight);
	glLoadMatrixf(glm::value_ptr(view));

	glLightfv((GL_LIGHT0 + numberLight), GL_POSITION, glm::value_ptr(position));
	glLightfv((GL_LIGHT0 + numberLight), GL_COLOR, glm::value_ptr(color));

	glLightfv((GL_LIGHT0 + numberLight), GL_AMBIENT, glm::value_ptr(glm::vec4(color, 1.0f)));
	glLightfv((GL_LIGHT0 + numberLight), GL_DIFFUSE, glm::value_ptr(glm::vec4(color, 1.0f)));
	glLightfv((GL_LIGHT0 + numberLight), GL_SPECULAR, glm::value_ptr(glm::vec4(color, 1.0f)));

	if (this->light_type == SPOT) { //if spotlight
		glLightfv((GL_LIGHT0 + numberLight), GL_SPOT_CUTOFF, &cutoff);
		glLightfv((GL_LIGHT0 + numberLight), GL_SPOT_DIRECTION, glm::value_ptr(direction));
	}
	else if (this->light_type == DIRECTIONAL) { //if directional
		glLightfv((GL_LIGHT0 + numberLight), GL_SPOT_DIRECTION, glm::value_ptr(direction));
	}
}