#pragma once

class LIB_API Node : public Object
{
public:
	Node();
	virtual ~Node();

	//render di Object
	void render(glm::mat4 view) override;

	Node* findByName(std::string name);

	inline void setPosition(glm::mat4 matrix) { 
		this->positionMatrix = matrix; };
	inline glm::mat4 getPosition() { return positionMatrix; }

	inline unsigned int getNrOfChildren() { return children.size(); };
	
	inline std::list<Node*> getChildren() { return children; }
	bool isRoot();
	void link(Node* child);
	void unlink();
	glm::mat4 getFinalMatrix() const;
	void calculateFinalMatrix();

	//controlla il valore z (distanza)
	bool operator > (const Node& node) const
	{
		return (finalPositionMatrix[3][2] > node.getFinalMatrix()[3][2]);
	}

protected:
	glm::mat4 positionMatrix;
	glm::mat4 finalPositionMatrix;
	Node* parent{ NULL };
	std::list<Node*> children;
};

