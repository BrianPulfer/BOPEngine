
#include <iostream>
#include "bopEngine.h"

//Namespaces
using namespace std;

//Cameras
bool cameraFree = true;
Camera* camera = new Camera();
Camera* cameraTop = new Camera();
Camera* cameraCorner = new Camera();
Camera* cameraCornerBehind = new Camera();
Camera* cameras[] = {camera, cameraTop, cameraCorner, cameraCornerBehind};
int cameraSelector = 0;
float boundX = 515.0f, boundZ_backwards = 24.0f, boundZ_forward = 1020.0f;
float cameraX = 0.0f;
float cameraY = 500.0f;
float cameraZ = 500.0f;
float step = 10.0f;
float cameraXMAX = cameraX + boundX, cameraXMIN = cameraX - boundX;
float cameraYMAX = 650;
float cameraYMIN = 0;
float cameraZMAX = cameraZ + boundZ_backwards, cameraZMIN = cameraZ - boundZ_forward;

//Dynamic light
Node* dynamicLight = nullptr;
float lightStep = 20.0f;

//List
List* sceneList = new List();

//Root
Node* root = new Node();

//Fingers
Node* index01 = nullptr;
Node* index02 = nullptr;
Node* index03 = nullptr;
Node* middle01 = nullptr;
Node* middle02 = nullptr;
Node* middle03 = nullptr;
Node* ring01 = nullptr;
Node* ring02 = nullptr;
Node* ring03 = nullptr;
Node* little01 = nullptr;
Node* little02 = nullptr;
Node* little03 = nullptr;
Node* thumb01 = nullptr;
Node* thumb02 = nullptr;
float angleRotationIndex = 0.0f;
float angleRotationMiddle = 0.0f;
float angleRotationRing = 0.0f;
float angleRotationLittle = 0.0f;
float angleRotationThumb = 0.0f;
float angleStep = 4.0f;
float angleControlIndex = 0.0f;
float angleControlMiddle = 0.0f;
float angleControlRing = 0.0f;
float angleControlLittle = 0.0f;
float angleControlThumb = 0.0f;
bool upIndex = false;
bool downIndex = true;
bool rotateIndex = false;
bool rotateMiddle = false;
bool rotateRing = false;
bool rotateLittle = false;
bool rotateThumb = false;
int yTemp = 0;

//Finger constrains
#define FINGER_LOW_CONSTRAIN	-110
#define FINGER_HIGH_CONSTRAIN	-4
#define THUMB_LOW_CONSTRAIN		-80
#define THUMB_HIGH_CONSTRAIN	-4

/*
This functions is called recursively to populate the list and compute the final matrix for each node
node: the first time populateList is called node is the root then node is a child of the root
*/
void populateList(Node * node) {
	node->calculateFinalMatrix();
	sceneList->add(node);
	for (auto &node : node->getChildren()) {
		populateList(node);
	}
}

/*
This callback is used to handle keyboard commands
w: move camera forward
s: move camera backward
a: move camera left
d: move camera right
q: move camera up
e: move camera down
c: change camera
1: select index finger
2: select middle finger
3: select ring finger
4: select little finger
5: select thumb finger
*/
void keyboardCallback(unsigned char key, int mouseX, int mouseY) {
	switch (key) {

	//w W
	case 119:
	case 87:
		if (cameraFree) {
			if (cameraZ - step > cameraZMIN)
				cameraZ -= step;
		}
		break;

	//a A
	case 97:
	case 65:
		if (cameraFree) {
			if (cameraX - step > cameraXMIN)
				cameraX -= step;
		}
		break;

	//s S
	case 115:
	case 83:
		if (cameraFree) {
			if (cameraZMAX > cameraZ + step)
				cameraZ += step;
		}
		break;

	//d D
	case 100:
	case 68:
		if (cameraFree) {
			if (cameraXMAX > cameraX + step)
				cameraX += step;
		}
		break;
	//q Q
	case 113:
	case 81:
		if (cameraFree) {
			if (cameraYMAX > cameraY + step)
				cameraY += step;
		}
		break;

	//e E
	case 101:
	case 69:
		if (cameraFree) {
			if (cameraYMIN < cameraY - step)
				cameraY -= step;
		}
		break;

	//1
	case 49:
		rotateIndex = !rotateIndex;
		break;

	//2
	case 50:
		rotateMiddle = !rotateMiddle;
		break;

	//3
	case 51:
		rotateRing = !rotateRing;
		break;

	//4
	case 52:
		rotateLittle = !rotateLittle;
		break;

	//5
	case 53:
		rotateThumb = !rotateThumb;
		break;


	//c C
	case 99:
	case 67:
		if (cameraSelector < 3)
			cameraSelector++;
		else
			cameraSelector = 0;

		cameraFree = (cameraSelector == 0) ? true : false;
		bopEngine::setCamera(cameras[cameraSelector]);
		break;
	}

}
/*
This callback is used to handle mouse movements
When fingers are selected and mouse goes up fingers open
When fingers are selected and mouse goes down fingers close
angleRotation is used inside the display callback to rotate the fingers
angleControll is used to constraints fingers movements
*/
void mouseMotionCallback(int x, int y) {
	//Index rotation
	if (rotateIndex && yTemp < y) {
		if (angleControlIndex >= FINGER_LOW_CONSTRAIN) {
			angleRotationIndex -= angleStep;
			angleControlIndex -= angleStep;
		}
	}
	if (rotateIndex && yTemp > y) {
		if (angleControlIndex <= FINGER_HIGH_CONSTRAIN) {
			angleRotationIndex += angleStep;
			angleControlIndex += angleStep;
		}
	}

	//Middle rotation
	if (rotateMiddle && yTemp < y) {
		if (angleControlMiddle >= FINGER_LOW_CONSTRAIN) {
			angleRotationMiddle -= angleStep;
			angleControlMiddle -= angleStep;
		}
	}
	if (rotateMiddle && yTemp > y) {
		if (angleControlMiddle <= FINGER_HIGH_CONSTRAIN) {
			angleRotationMiddle += angleStep;
			angleControlMiddle += angleStep;
		}
	}

	//Ring rotation
	if (rotateRing && yTemp < y) {
		if (angleControlRing >= FINGER_LOW_CONSTRAIN) {
			angleRotationRing -= angleStep;
			angleControlRing -= angleStep;
		}
	}
	if (rotateRing && yTemp > y) {
		if (angleControlRing <= FINGER_HIGH_CONSTRAIN) {
			angleRotationRing += angleStep;
			angleControlRing += angleStep;
		}
	}

	//Little rotation
	if (rotateLittle && yTemp < y) {
		if (angleControlLittle >= FINGER_LOW_CONSTRAIN) {
			angleRotationLittle -= angleStep;
			angleControlLittle -= angleStep;
		}
	}
	if (rotateLittle && yTemp > y) {
		if (angleControlLittle <= FINGER_HIGH_CONSTRAIN) {
			angleRotationLittle += angleStep;
			angleControlLittle += angleStep;
		}
	}

	//Thumb rotation
	if (rotateThumb && yTemp < y) {
		if(angleControlThumb >= THUMB_LOW_CONSTRAIN) {
			angleRotationThumb -= angleStep;
			angleControlThumb -= angleStep;
		}
	}
	if (rotateThumb && yTemp > y) {
		if(angleControlThumb <= THUMB_HIGH_CONSTRAIN) {
			angleRotationThumb += angleStep;
			angleControlThumb += angleStep;
		}
	}

	yTemp = y;
}

/*
This callback is used to handle display movements each frame
- fingers movements
- camera movements
- dynamic light movements
- rendering scene
*/

void displayCallback() {
	//Fingers movements
	index01->setPosition(glm::mat4(glm::rotate(index01->getPosition(), glm::radians(angleRotationIndex), glm::vec3(1.0f, 0.0f, 0.0f))));
	index02->setPosition(glm::mat4(glm::rotate(index02->getPosition(), glm::radians(angleRotationIndex), glm::vec3(1.0f, 0.0f, 0.0f))));
	index03->setPosition(glm::mat4(glm::rotate(index03->getPosition(), glm::radians(angleRotationIndex), glm::vec3(1.0f, 0.0f, 0.0f))));
	angleRotationIndex = 0;
	middle01->setPosition(glm::mat4(glm::rotate(middle01->getPosition(), glm::radians(angleRotationMiddle), glm::vec3(1.0f, 0.0f, 0.0f))));
	middle02->setPosition(glm::mat4(glm::rotate(middle02->getPosition(), glm::radians(angleRotationMiddle), glm::vec3(1.0f, 0.0f, 0.0f))));
	middle03->setPosition(glm::mat4(glm::rotate(middle03->getPosition(), glm::radians(angleRotationMiddle), glm::vec3(1.0f, 0.0f, 0.0f))));
	angleRotationMiddle = 0;
	ring01->setPosition(glm::mat4(glm::rotate(ring01->getPosition(), glm::radians(angleRotationRing), glm::vec3(1.0f, 0.0f, 0.0f))));
	ring02->setPosition(glm::mat4(glm::rotate(ring02->getPosition(), glm::radians(angleRotationRing), glm::vec3(1.0f, 0.0f, 0.0f))));
	ring03->setPosition(glm::mat4(glm::rotate(ring03->getPosition(), glm::radians(angleRotationRing), glm::vec3(1.0f, 0.0f, 0.0f))));
	angleRotationRing = 0;
	little01->setPosition(glm::mat4(glm::rotate(little01->getPosition(), glm::radians(angleRotationLittle), glm::vec3(1.0f, 0.0f, 0.0f))));
	little02->setPosition(glm::mat4(glm::rotate(little02->getPosition(), glm::radians(angleRotationLittle), glm::vec3(1.0f, 0.0f, 0.0f))));
	little03->setPosition(glm::mat4(glm::rotate(little03->getPosition(), glm::radians(angleRotationLittle), glm::vec3(1.0f, 0.0f, 0.0f))));
	angleRotationLittle = 0;
	thumb01->setPosition(glm::mat4(glm::rotate(thumb01->getPosition(), glm::radians(angleRotationThumb), glm::vec3(0.0f, 1.0f, 1.0f))));
	thumb02->setPosition(glm::mat4(glm::rotate(thumb02->getPosition(), glm::radians(angleRotationThumb*2.0f), glm::vec3(0.0f, 1.0f, 1.0f))));
	angleRotationThumb = 0;

	//Camera movements
	camera->setViewMatrix(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(0.0f, 200.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//Dynamic light movements
	if (dynamicLight->getFinalMatrix()[3].x > 400)
		lightStep *= -1;
	if (dynamicLight->getFinalMatrix()[3].x < -400)
		lightStep *= -1;
	dynamicLight->setPosition(glm::translate(dynamicLight->getPosition(), glm::vec3(lightStep, 0.0f, 0.0f)));

	/*
	This sequence is used to render the scene
	First the root's matrix is scaled on the y axis so that the scene is rendered upside down
	Then the scene list is flushed
	Finally the root's matrix is reset as original, the list is repopulated and the scene is rendered normally
	Then the scene list is flushed once again
	Swapbuffer is called at the and by the engine
	*/
	root->setPosition((glm::scale(glm::mat4(), glm::vec3(1.0f, -1.0f, 1.0f))));
	populateList(root);
	bopEngine::render(sceneList);

	sceneList->flush();

	root->setPosition(glm::mat4(1.0f));
	populateList(root);
	bopEngine::render(sceneList);

	sceneList->flush();

}

/*
This callback is used to handle timer callback
This callback must be registered to show fps on the window even if is empty
How often this callback is invoked is set to 1 second by the Engine
*/
void timerCallback(int val) {

}


int main(int argc, char *argv[])
{
	//Init context
	bopEngine::init(argc, argv);
	bopEngine::load("../files/GuantoBOP.OVO");
	root = bopEngine::getNodeByName("[root]");

	//Meshes with transparency
	Node* pavimento = bopEngine::getNodeByName("Pavimento");
	Node* piedistallo = bopEngine::getNodeByName("Piedistallo");
	dynamic_cast<Mesh*>(pavimento)->getMaterial()->setTransparency(0.8f);
	dynamic_cast<Mesh*>(piedistallo)->getMaterial()->setTransparency(0.4f);

	//Fingers
	index01 = bopEngine::getNodeByName("indice01");
	index02 = bopEngine::getNodeByName("indice02");
	index03 = bopEngine::getNodeByName("indice03");
	middle01 = bopEngine::getNodeByName("medio01");
	middle02 = bopEngine::getNodeByName("medio02");
	middle03 = bopEngine::getNodeByName("medio03");
	ring01 = bopEngine::getNodeByName("anulare01");
	ring02 = bopEngine::getNodeByName("anulare02");
	ring03 = bopEngine::getNodeByName("anulare03");
	little01 = bopEngine::getNodeByName("mignolo01");
	little02 = bopEngine::getNodeByName("mignolo02");
	little03 = bopEngine::getNodeByName("mignolo03");
	thumb01 = bopEngine::getNodeByName("pollice01");
	thumb02 = bopEngine::getNodeByName("pollice02");

	//Dynamic light
	dynamicLight = bopEngine::getNodeByName("dynaLight");

	//Camera settings
	camera->setViewMatrix(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	cameraTop->setViewMatrix(glm::vec3(0.0f, 650.0f, 150.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	cameraCorner->setViewMatrix(glm::vec3(500.0f, 225.0f, 450.0f), glm::vec3(0.0f, 275.0f, 0.0), glm::vec3(0.0f, 1.0f, 0.0f));
	cameraCornerBehind->setViewMatrix(glm::vec3(-500.0f, 225.0f, -450.0f), glm::vec3(0.0f, 275.0f, 0.0), glm::vec3(0.0f, 1.0f, 0.0f));
	bopEngine::setCamera(cameras[cameraSelector]);

	//Callbacks
	bopEngine::registerDisplayCallback(displayCallback);
	bopEngine::registerKeyBoardCallback(keyboardCallback);
	bopEngine::registerTimerCallback(timerCallback); 
	bopEngine::registerMouseMotionCallback(mouseMotionCallback);

	cout << endl;
	cout << "****************************COMMANDS****************************" << endl;
	cout << "w : move camera forward" << endl;
	cout << "s : move camera backward" << endl;
	cout << "a : move camera left" << endl;
	cout << "d : move camera right" << endl;
	cout << "c : change camera" << endl;
	cout << "q : move camera up" << endl;
	cout << "e : move camera down" << endl;
	cout << "1 : select index finger" << endl;
	cout << "2 : select middle finger" << endl;
	cout << "3 : select ring finger" << endl;
	cout << "4 : select little finger" << endl;
	cout << "5 : select thumb finger" << endl;
	cout << "press left/righ click + mouse down --> move fingers down" << endl;
	cout << "press left/righ click + mouse up --> move fingers down" << endl;
	cout << "****************************************************************" << endl;

	//Enter main loop
	bopEngine::enterMainLoop();

	//Free the engine
	bopEngine::free();

	for (auto &camera : cameras) {
		delete camera;
	}
	cout << "Avvio di clientApp" << endl;
}
