/**
 * @file        bopEngine.h
 * @author        Patrick Valnegri, Brian Pulfer, Olmo Barberis
 */
#pragma once
// Generic info:
#define LIB_NAME "BOPEngine" ///< Library credits
#define LIB_VERSION 10		 ///< Library version (divide by 10)
#ifdef _WINDOWS
#include <windows.h>
// Export API:
// Specifies i/o linkage (VC++ spec):
#ifdef BOPENGINE_EXPORTS
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else			// Under Linux
#define LIB_API // Dummy declaration
#endif
//Include
//C++/C
#include <string>
#include <list>
#include <iostream>
#include <vector>
#include <math.h>
// GLM:
#define GLM_FORCE_RADIANS
#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/packing.hpp>
//Classes
#include "vertex.h"
#include "face.h"
#include "object.h"
#include "node.h"
#include "texture.h"
#include "material.h"
#include "camera.h"
#include "light.h"
#include "list.h"
#include "mesh.h"
//Define
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
//Class Bopengine
class LIB_API bopEngine
{
  public:
	//Ovoreader
	static Node *readChunk(FILE *dat, unsigned int position); //recursively read
	static Node *load(const char *src);						  //load scene

	//Texture
	static Texture *buildTexture(std::string path);

	//Freeglut contest functions mapped
	static bool init(int argc, char *argv[]); //init contest,
	static bool enterMainLoop();			  //enter main loop
	static bool clear();					  //clear buffers and colors

	//Camera
	static void setCamera(Camera *camera); //set camera position
	static Camera *getCamera();			   //get camera position

	// Free
	static bool free(); //free contest

	//Nodes
	static Node *getRoot();						  //get node root
	static Node *getNodeByName(std::string name); //get node by name

	//glut register function mapped
	static bool registerDisplayCallback(void displayCallback(void));					  //register display callback
	static bool registerSpecialCallback(void specialCallback(int, int, int));			  //register special callback
	static bool registerKeyBoardCallback(void keyboardCallback(unsigned char, int, int)); //register keyboard callback
	static bool registerMouseCallback(void mouseCallback(int, int, int, int));			  //register mouse callback
	static bool registerMouseMotionCallback(void mouseMotionCallback(int x, int y));	  // register mouse motion callback
	static bool registerTimerCallback(void timerCallback(int));							  //register timer callback

	//Anisotropic values
	static bool isAnisotropicSupported; //is supported or not
	static int anisotropicLevel;		//level of anisotropic

	//Render method
	static void render(List *list); //render scene from list

  private:
	//User callbacks
	static void (*userDisplayCallback)();						  //user display callback
	static void (*userKeyboardCallback)(unsigned char, int, int); //user keyboard callback
	static void (*userSpecialCallback)(int, int, int);			  //user special callback
	static void (*userTimerCallback)(int);						  //user timer callback

	//Engine callbacks
	static void bopEngineDisplayCallback();							//engine display callback
	static void bopEngineKeyboardCallback(unsigned char, int, int); //engine keyboard callback
	static void bopEngineReshapeCallback(int, int);					//engine reshape callback
	static void bopEngineTimerCallback(int);						//engine timer callback
	static void bopEngineSpecialCallback(int, int, int);			//engine special callback

	//Private functions
	static int fps;		   //current
	static int frames;	 //variable to
	static void showFPS(); //show fps on screen

	//Fields
	static glm::mat4 perspective;			  //perspective camera
	static glm::mat4 ortho;					  //ortho camera
	static Node *root;						  //root of the scene
	static Camera *currentCamera;			  //current camera
	static std::vector<Material *> materials; //list of materials
	static std::vector<Texture *> textures;   //list of textures
	static bool initFlag;					  //avoid double inits
	static int windowID;					  //to force rendering refresh
	static int windowWidth;
	static int windowHeight;
};