#include <iostream>
#include <assert.h>
#include "bopEngine.h"
#include <FreeImage.h>

#include "testUtilities.h"

using namespace std;

void displayCallback() {}
void specialCallback(int, int, int) {}
void keyboardCallback(unsigned char, int, int) {}
void mouseCallback(int, int, int, int) {}
void timerCallback(int) {}
void idleFuncCallBack() {}

int main(int argc, char *argv[])
{
	cout << "=========================================" << endl;
	cout << "Avvio client test" << endl;
	cout << "=========================================" << endl;

	//Assertion methods to test bopEngine class
	/**********************BOPENGINE***************************/
	cout << "=========================================" << endl;
	cout << "Start bopEngine class tests" << endl;
	cout << "=========================================" << endl;

	cout << "cant enter main loop before init contest" << endl;
	assert((false == bopEngine::enterMainLoop()));
	cout << "cant clear before init contest" << endl;
	assert((false == bopEngine::clear()));
	cout << "cant free before init contest" << endl;
	assert((false == bopEngine::free()));

	cout << "cant load a displayCallback before init contest" << endl;
	assert((false == bopEngine::registerDisplayCallback(displayCallback)));
	cout << "cant load a specialCallback before init contest" << endl;
	assert((false == bopEngine::registerSpecialCallback(specialCallback)));
	cout << "cant load a keyboardCallback before init contest" << endl;
	assert((false == bopEngine::registerKeyBoardCallback(keyboardCallback)));
	cout << "cant load a mouseCallback before init contest" << endl;
	assert((false == bopEngine::registerMouseCallback(mouseCallback)));
	cout << "cant load a timerCallback before init contest" << endl;

	assert((false == bopEngine::registerTimerCallback(timerCallback)));

	cout << "Contest initialized" << endl;
	bopEngine::init(argc, argv);

	cout << "can register a displayCallback after init contest" << endl;
	assert((true == bopEngine::registerDisplayCallback(displayCallback)));
	cout << "can load a specialCallback after init contest" << endl;
	assert((true == bopEngine::registerSpecialCallback(specialCallback)));
	cout << "can load a keyboardCallback after init contest" << endl;
	assert((true == bopEngine::registerKeyBoardCallback(keyboardCallback)));
	cout << "can load a mouseCallback after init contest" << endl;
	assert((true == bopEngine::registerMouseCallback(mouseCallback)));
	cout << "can load a timerCallback after init contest" << endl;
	assert((true == bopEngine::registerTimerCallback(timerCallback)));

	cout << "Scene loaded" << endl;
	bopEngine::load("../files/guantoBOP.OVO");

	cout << "can enter main loop after init contest - close window to continue test" << endl;
	assert((true == bopEngine::enterMainLoop()));
	cout << "can clear scene after init contest" << endl;
	assert((true == bopEngine::clear()));

	//Assertion methods to test class node
	/**************************NODE****************************/
	cout << "Node founded" << endl;

	Node *root = bopEngine::getRoot();
	assert(root->isRoot());

	Node *palmo = bopEngine::getNodeByName("palmo");
	assert(palmo != nullptr);
	assert(!palmo->isRoot());

	//Root has 2 childs: piedistallo, polso and Luce statica. Polso only has one child.
	assert(palmo->getNrOfChildren() == 10);

	Node *random = new Node();
	assert(random->getNrOfChildren() == 0);

	glm::mat4 palmo_position = (glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -200.0f)));
	glm::mat4 random_position = (glm::translate(glm::mat4(), glm::vec3(0.0f, 100.0f, 0.0f)));

	root->setPosition(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -0.0f)));
	palmo->setPosition(palmo_position);
	random->setPosition(random_position);

	palmo->link(random);
	assert(palmo->getNrOfChildren() == 11);

	root->calculateFinalMatrix();
	palmo->calculateFinalMatrix();
	random->calculateFinalMatrix();

	random->unlink();
	assert(palmo->getNrOfChildren() == 10);

	cout << "Node correctly not founded" << endl;
	Node *drago = bopEngine::getNodeByName("Drago");
	assert(drago == nullptr);

	cout << "can free scene after init contest" << endl;
	assert((true == bopEngine::free()));

	cout << "=========================================" << endl;
	cout << "End bopEngine class tests" << endl;
	cout << "=========================================" << endl;

	////Assertion methods to test class camera
	///*************************CAMERA***************************/
	Camera *camera = bopEngine::getCamera();

	glm::mat4 firstViewMatrix = camera->getViewMatrix();
	//camera->keyPressed('w');
	assert(firstViewMatrix == camera->getViewMatrix());
	////Assertion methods to test class light
	///*************************LIGHT****************************/

	glm::vec4 paramLight(1.0f, 1.0f, 1.0f, 1.0f);

	Light light{};

	//check light type
	cout << "Light 'SPOT' - Cutoff == 15" << endl;
	light.setLightType(SPOT);
	assert(SPOT, light.getLightType);
	assert(15.0f, light.getCutoff);

	/**********************************************************/

	//Assertion methods to test class list
	/**************************LIST***************************/
	List *list = new List();
	list->add(random);

	/**********************************************************/

	//Assertion methods to test class material
	///*************************TEXTURE*************************/
	unsigned char *bitmap = (unsigned char *)malloc(sizeof(unsigned char) * 1000);

	for (int i = 0; i < 1000; i++)
	{
		bitmap[i] = i;
	}

	FIBITMAP *fibitmap = new FIBITMAP();
	fibitmap->data = bitmap;

	Texture *texture = new Texture();
	texture->setBitmap(fibitmap);

	assert(texture->getBitmap() == fibitmap);

	///*************************MATERIAL*************************/
	glm::vec3 paramMaterial = glm::vec3(128.0f, 64.0f, 0.0f);
	Material material{};
	material.setProperties(paramMaterial); //Red material

	assert((paramMaterial.x * 0.2f == material.getAmbient().x));
	assert((paramMaterial.y * 0.2f == material.getAmbient().y));
	assert((paramMaterial.z * 0.2f == material.getAmbient().z));

	assert((paramMaterial.x * 0.4f == material.getSpecular().x));
	assert((paramMaterial.y * 0.4f == material.getSpecular().y));
	assert((paramMaterial.z * 0.4f == material.getSpecular().z));

	assert((paramMaterial.x * 0.6f == material.getDiffuse().x));
	assert((paramMaterial.y * 0.6f == material.getDiffuse().y));
	assert((paramMaterial.z * 0.6f == material.getDiffuse().z));

	material.setShininess(128);
	assert(128, material.getShininess());

	material.setTexture(texture);
	assert(texture == material.getTexture());

	/**********************************************************/

	//Assertion methods to test class vertex
	/**************************VERTEX****************************/
	Vertex *vertex1 = new Vertex();
	Vertex *vertex2 = new Vertex();
	Vertex *vertex3 = new Vertex();

	glm::vec3 v1 = glm::vec3(-5, 0, 0);
	glm::vec3 v2 = glm::vec3(5, 0, 0);
	glm::vec3 v3 = glm::vec3(0, 7, 0);

	vertex1->setCoordinates(v1);
	vertex2->setCoordinates(v2);
	vertex3->setCoordinates(v3);

	vertex1->setNormal(glm::vec4(0, 1, 0, 0));
	vertex2->setNormal(glm::vec4(0, 1, 0, 0));
	vertex3->setNormal(glm::vec4(0, 1, 0, 0));

	//Random texture assignment
	vertex1->setTextureCoordinates(glm::vec2(0, 0));
	vertex2->setTextureCoordinates(glm::vec2(0, 1));
	vertex3->setTextureCoordinates(glm::vec2(1, 0));

	Vertex *vertex_array[] = {vertex1, vertex2, vertex3};
	glm::vec3 v_array[] = {v1, v2, v3};

	for (int i = 0; i < 3; i++)
	{
		assert(vertex_array[i]->getCoordinates() == v_array[i]);
		assert(vertex_array[0]->getNormal() == glm::vec4(0, 1, 0, 0));
	}

	assert(vertex1->getTextureCoordinates() == glm::vec2(0, 0));
	assert(vertex2->getTextureCoordinates() == glm::vec2(0, 1));
	assert(vertex3->getTextureCoordinates() == glm::vec2(1, 0));

	//Assertion methods to test class face
	/**************************FACE****************************/
	Face *face = new Face();
	face->setFace(vertex1, vertex2, vertex3);

	assert(face->getVert1() == vertex1);
	assert(face->getVert2() == vertex2);
	assert(face->getVert3() == vertex3);

	//Assertion methods to test class mesh
	/**************************MESH****************************/
	Mesh *mesh = new Mesh();

	mesh->setMaterial(&material);
	mesh->addFace(face);

	/**********************************************************/

	/**********************************************************/

	//Assertion methods to test class object
	/**************************OBJECT**************************/

	/**********************************************************/

	//Assertion methods to test class rowList
	/**************************ROWLIST*************************/

	/**********************************************************/
	//Assertion methods to test class texture
	/**************************TEXTURE*************************/

	/**********************************************************/
	std::cout << "=========================================" << std::endl;
	std::cout << "Test Passed" << std::endl;
	std::cout << "=========================================" << std::endl;
	return 0;
}