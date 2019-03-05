#pragma once

class LIB_API Material : public Object
{
public:
	Material();
	~Material();

	
	void render(glm::mat4 view) override;

	void setProperties(glm::vec3 albedo);
	//set shininess value
	void setShininess(float roughness);
	
	inline void setTexture(Texture* texture) { this->texture = texture; }

	//return ambient vec
	inline glm::vec4 getAmbient() { return ambient; }
	//return diffuse vec
	inline glm::vec4 getDiffuse() { return diffuse; }
	//return specular vec
	inline glm::vec4 getSpecular() { return specular; }
	//return shininess values
	inline int getShininess() { return shininess; }
	inline Texture* getTexture() { return texture; }
	inline void setTransparency(float transparency) { this->transparency = transparency; }
	inline float getTransparency() { return transparency; }


private:
	//proprerties
	Texture* texture;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec3 albedo;
	float shininess;
	float transparency;
};

