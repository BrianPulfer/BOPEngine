#include "bopEngine.h"

// FreeGLUT:   
#include <GL/freeglut.h>
#include <fstream>

LIB_API Texture::Texture()
{
	glGenTextures(1, &currentTextureID);
}
LIB_API Texture::~Texture()
{
	FreeImage_Unload(bitmap);
}

void Texture::render(glm::mat4 view)
{
	glBindTexture(GL_TEXTURE_2D, currentTextureID);
}
