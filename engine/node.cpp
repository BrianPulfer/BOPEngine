#include "bopEngine.h"

LIB_API Node::~Node()
{

	for (auto &node : children) {
		delete node;
	}
}

LIB_API Node::Node()
{

}


void Node::render(glm::mat4 view)
{
	for (auto &node : children) {
		node->render(view);
	}
}

Node * Node::findByName(std::string name)
{
	if (this->getName() == name) {
		return this;
	}
	else {
		bool notFound = true;
		Node* returned;
		for (auto &child : children) {
			returned = child->findByName(name);
			if (returned != nullptr)
				return returned;
		}
		return nullptr;
	}
}

bool LIB_API Node::isRoot()
{
	if (parent == nullptr)
		return true;
	return false;
}

//Add a parent to the node
void LIB_API Node::link( Node* child)
{
	if (child == nullptr)
		return;
	children.push_back(child);
	child->parent = this;
}

//Remove from parent's childern list the actual node and set parent = null
void LIB_API Node::unlink()
{
	parent->children.remove(this);
	this->parent = nullptr;
}

glm::mat4 LIB_API Node::getFinalMatrix() const
{
	return finalPositionMatrix;
}


void Node::calculateFinalMatrix()
{
	if (!isRoot())
		this->finalPositionMatrix = parent->getFinalMatrix() * positionMatrix;
	else
		this->finalPositionMatrix = positionMatrix;
}

