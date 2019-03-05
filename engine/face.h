#pragma once
class LIB_API Face
{
public:
	Face();
	~Face();

	void render(glm::mat4 view);

	inline Vertex* getVert1() { return vert1; };
	inline Vertex* getVert2() { return vert2; };
	inline Vertex* getVert3() { return vert3; };

	inline void setFace(Vertex* v1, Vertex* v2, Vertex* v3) {
		vert1 = v1;
		vert2 = v2;
		vert3 = v3;
	};

private:
	Vertex* vert1;
	Vertex* vert2;
	Vertex* vert3;
};

