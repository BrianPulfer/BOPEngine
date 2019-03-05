#include "bopEngine.h"
#include <algorithm>
#include <GL/freeglut.h>
#include <functional>

LIB_API List::List()
{
}

LIB_API List::~List()
{
}

/*
Rendering order:
- Lights are rendered first
- Then the solid meshes are rendered
- The transparent nodes are sorted in order from farthest to nearest
- The transparent nodes are rendered in two part: 
	- first the backfaces 
	- then the frontfaces.
*/
void List::render(glm::mat4 view)
{
	for (auto &light : lights) {
		light->render(view);
	}
	for (auto &node : nodes) {
		node->render(view);
	}
	
	sortTransparentNode();

	//enable FaceCulling only for the transparent nodes
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//The backfaces are rendered first using GL_FRONT
	glCullFace(GL_FRONT);
	for (auto &node : transparentNodes) {
		node->render(view);
	}
	//Faceculling mode are switched (GL_BACK) to render the frontfaces
	glCullFace(GL_BACK);
	for (auto &node : transparentNodes) {
		node->render(view);
	}
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);


}

/*
Method that add lights and meshes to the list, others object are ignored.
- First the lights are added to the 'lights' vector. Only 8 lights are added to the list
- Then the meshes are added to the list in two pieces:
	- if solid the mesh is added to the 'nodes' vector
	- if transparent the mesh is added to the 'transparentNodes' vector
*/
void List::add(Node * node)
{
	//If node is not a light the dynamic cast return a nullptr
	if (dynamic_cast<Light*>(node) != nullptr && nrLights < 8) {
		lights.push_back(dynamic_cast<Light *>(node));
		nrLights++;
		return;
	}

	//If node is not a mesh the dynamic cast return a nullptr
	Mesh *mesh = dynamic_cast<Mesh*>(node);
	if (mesh != nullptr) {
		if (mesh->getMaterial()->getTransparency() == 1) {
			nodes.push_back(mesh);
		}
		else {
			transparentNodes.push_back(mesh);
		}
	}
}

void List::flush()
{
	//Lights are turned off before flushing the list
	for (auto &light : lights) {
		light->turnOff();
	}

	while (lights.size() > 0) {
		lights.pop_back();
	}
	nrLights = 0;

	while (transparentNodes.size() > 0)
		transparentNodes.pop_back();

	while(nodes.size() > 0) {
		nodes.pop_back();
	}

}
//Method that sort transparentNodes from farthest to nearest
void List::sortTransparentNode()
{
	std::sort(transparentNodes.begin(), transparentNodes.end(), std::greater<Node*>());
}

//Compare use the '>' operator which is overridden in the Node class
bool List::comparePtrToNode(Node * a, Node * b)
{
	return (*a > *b);
}
