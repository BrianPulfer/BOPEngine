#pragma once

class LIB_API Camera : public Node
{
public:
	Camera();
	~Camera();

	inline void render(glm::mat4)override {};
	void setViewMatrix(glm::vec3 eye, glm::vec3 center, glm::vec3 up);
	void setViewMatrix(glm::mat4 view);

	inline glm::mat4 getViewMatrix() { return viewMatrix; };

private:

	glm::mat4 viewMatrix;
};

