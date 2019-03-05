#pragma once


#include <FreeImage.h>

#define GL_TEXTURE_MAX_ANISOTROPY_EXT        0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT    0x84FF


class LIB_API Texture : public Object
{
public:
	Texture();
	~Texture();

	void render(glm::mat4 view) override;
	inline void setBitmap(FIBITMAP *bmp) { this->bitmap = bmp; };
	inline FIBITMAP *getBitmap() { return bitmap; };
	inline int getTextureId() { return currentTextureID; };

private:
	FIBITMAP *bitmap;
	unsigned int currentTextureID;

};

