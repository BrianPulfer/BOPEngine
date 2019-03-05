#pragma once

class LIB_API Mesh : public Node
{
public:
	Mesh();
	~Mesh();

	void render(glm::mat4 view) override;

	void setMaterial(Material* mat) { material = mat;  };
	inline Material* getMaterial() { return material; }
	void addFace(Face* face) { faces.push_back(face); };
		
private:
	Material* material;
	std::vector<Face*> faces;
};

