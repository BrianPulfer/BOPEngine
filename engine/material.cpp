#include "bopEngine.h"

// FreeGLUT:   
#include <GL/freeglut.h>


LIB_API Material::Material()
{
}

LIB_API Material::~Material()
{
}

void LIB_API Material::render(glm::mat4 view) 
{
	setProperties(albedo);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glm::value_ptr(this->ambient));
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glm::value_ptr(this->diffuse));
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glm::value_ptr(this->specular));
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, this->shininess);
	
	if (this->texture != nullptr) {
		this->texture->render(view);
	}
}


void LIB_API Material::setProperties(glm::vec3 albedo)
{
	this->ambient = glm::vec4(albedo * 0.2f, transparency);
	this->specular = glm::vec4(albedo * 0.4f, transparency);
	this->diffuse = glm::vec4(albedo * 0.6f, transparency);
	this->albedo = albedo;
}

void LIB_API Material::setShininess(float roughness)
{
	this->shininess = (1 - sqrt(roughness)) * 128;
}
