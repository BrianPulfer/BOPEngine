#include "bopEngine.h"

// FreeGLUT:   
#include <GL/freeglut.h>

LIB_API Mesh::Mesh()
{
}


LIB_API Mesh::~Mesh()
{
}

void LIB_API Mesh::render(glm::mat4 view)
{
	glm::mat4 position = view * finalPositionMatrix;

	glLoadMatrixf(glm::value_ptr(position));

	if(this->material != nullptr)
		this->material->render(view);

	if (this->getMaterial() != nullptr && this->getMaterial()->getTexture() != nullptr) {
		glEnable(GL_TEXTURE_2D);
	}

	for (auto &face : faces) {
		face->render(view);
	}

	if (this->getMaterial() != nullptr && this->getMaterial()->getTexture() != nullptr) {
		glDisable(GL_TEXTURE_2D);
	}
}