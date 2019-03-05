#pragma once

typedef enum : int {
	OMNI =0 , DIRECTIONAL, SPOT
} LIGHT_TYPE;

class LIB_API Light : public Node
{
public:
	Light();
	~Light();

	void turnOff();

	//Set light type: Directional, Omnidirectional or Spot
	void setLightType(LIGHT_TYPE light_type);

	//Set light's matrixes
	inline void setColor(glm::vec3 col) { this->color = col; }



	void setDirection(glm::vec3 direction);
	//Set cutoff when light type is spot
	void setCutoff(float cutoff);

	inline void setRadius(float lightRadius) { this->lightRadius = lightRadius; };

	//return current cutoff
	inline float getCutoff() { return cutoff; };
	//return current number light
	inline int getNumberLight() { return numberLight; };
	//return light type
	inline LIGHT_TYPE getLightType() { return light_type; }

	void render(glm::mat4 view) override;


private:

	static int numberLights;
	int numberLight;
	LIGHT_TYPE light_type;
	glm::vec3 color;
	glm::vec3 direction;

	//lightRadius is not used
	float lightRadius, cutoff;
};

