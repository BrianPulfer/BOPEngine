#pragma once
class LIB_API Vertex
{
public:
	Vertex();
	~Vertex();
	
	inline	glm::vec3 getCoordinates() { return vertexCoordinates; };
	inline glm::vec4 getNormal() { return normal; };
	inline glm::vec2 getTextureCoordinates() { return textCoordinates; };
	inline unsigned int getId() { return id; };

	inline void setCoordinates(glm::vec3 coordinates) { vertexCoordinates = coordinates; };
	inline void setNormal(glm::vec4 norm) { normal = norm; };
	inline void setTextureCoordinates(glm::vec2 coord) { textCoordinates = coord; };

	inline void setId(unsigned int i) { id = i; };

private:
	glm::vec3 vertexCoordinates;
	glm::vec4 normal;
	glm::vec2 textCoordinates;
	unsigned int id;
};

