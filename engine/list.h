#pragma once

class LIB_API List : public Object
{
public:
	List();
	~List();

	void render(glm::mat4 view) override;
	void add(Node* node);
	void flush();

	void sortTransparentNode();

	bool comparePtrToNode(Node* a, Node* b);

private:
	std::vector<Node*> nodes;
	std::vector<Node*> transparentNodes;
	std::vector<Light*>lights;
	int nrLights = 0;
};

