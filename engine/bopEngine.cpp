/**
 * @file        bopEngine.cpp
 * @author        Patrick Valnegri, Brian Pulfer, Olmo Barberis
 */
 //Engine
#include "bopEngine.h"

//Freeimage
#include <FreeImage.h>

//FreeGLUT:
#include <GL/freeglut.h>

//global
char resourcesPath[] = "../files/";

//namespace
using namespace std;

//Define
#define GL_TEXTURE_MAX_ANISOTROPY_EXT        0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT    0x84FF
#define WINDOW_DEFAULT_WIDTH                 800.0f
#define WINDOW_DEFAULT_HEIGHT                600.0f
#define FAR_PLANE							3000.0f

//static initialization
bool bopEngine::initFlag = false;
std::vector<Material*> bopEngine::materials;
std::vector<Texture*> bopEngine::textures;
int bopEngine::windowID = 0;
Node* bopEngine::root{ new Node() };
Camera* bopEngine::currentCamera{ new Camera() };
glm::mat4 bopEngine::perspective = glm::mat4(0.0f);
glm::mat4 bopEngine::ortho = glm::mat4(0.0f);

bool bopEngine::isAnisotropicSupported = false;
int bopEngine::anisotropicLevel = 1;
int bopEngine::fps = 0;
int bopEngine::frames = 0;
int bopEngine::windowHeight = WINDOW_DEFAULT_HEIGHT;
int bopEngine::windowWidth = WINDOW_DEFAULT_WIDTH;

//User callbacks
void(*bopEngine::userDisplayCallback)() = nullptr;
void(*bopEngine::userKeyboardCallback)(unsigned char, int, int) = nullptr;
void(*bopEngine::userSpecialCallback)(int, int, int) = nullptr;
void(*bopEngine::userTimerCallback)(int) = nullptr;

//////////////
// DLL MAIN //
//////////////
#ifdef _WINDOWS
#include <Windows.h>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * DLL entry point. Avoid to rely on it for easier code portability (Linux doesn't use this method).
 * @param instDLL handle
 * @param reason reason
 * @param _reserved reserved
 * @return true on success, false on failure
 */
int APIENTRY DllMain(HANDLE instDLL, DWORD reason, LPVOID _reserved)
{
	// Check use:
	switch (reason)
	{

	case DLL_PROCESS_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	//Done:
	return true;
}
#endif

/*
This function is called for initializing the whole context
- FreeImage context
- freeGlut context
- set perpective and ortho matrix
- check if anisotropic is supported
- register internal reshape callback
return true if done correctly
*/
bool LIB_API bopEngine::init(int argc, char *argv[])
{
	//Singleton
	if (initFlag)
	{
		return false;
	}

	// Init FreeImage:
	FreeImage_Initialise();

	//Init FreeGlut
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	// GLUT_RGB -> alias for RGBA, GLUT_DOUBLE -> double buffered window, GLUT_DEPTH -> zBuffer

	glutInitWindowPosition(100, 100);
	glutInit(&argc, argv);

	//Options
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	//GLUT_ACTION_ON_WINDOW_CLOSE -> Controls what happens when a window is closed by the user or system
	//GLUT_ACTION_GLUTMAINLOOP_RETURNS -> will immediately return from the main loop

	// Create the window with a specific title:
	windowID = glutCreateWindow("bopEngine");

	//Set window dimension
	glutReshapeWindow(WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT);

	//Set perspective and ortho matrix
	//Perspective arguments: field of view in radians, aspect ratio, near plane, far plane
	perspective = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 1.0f, FAR_PLANE);
	//Ortho arguments: left, right, bottom, up, zNear, zFar
	ortho = glm::ortho(0.0f, WINDOW_DEFAULT_WIDTH, 0.0f, WINDOW_DEFAULT_HEIGHT, -1.0f, 1.0f);

	glEnable(GL_NORMALIZE); //Enable normal vectors
	glEnable(GL_DEPTH_TEST); //Enable zBuffer
	glClearDepth(1.0); //specify the clear value for the depth buffer (zBuffer)
	glDepthFunc(GL_LESS); //specify the value used for depth buffer comparisons (zBuffer)

	//Enable Lights
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH); //Activate Gouraud shading, better than GL_FLAT
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0f); //More accurate computation of specular highlights

	// Check for anistropic filtering extension:
	if ((strstr((const char *)glGetString(GL_EXTENSIONS), "GL_EXT_texture_filter_anisotropic")))
	{
		std::cout << " Anisotropic filtering supported" << std::endl;
		isAnisotropicSupported = true;
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropicLevel);
	}
	else
	{
		std::cout << " Anisotropic filtering not supported" << std::endl;
	}

	//Register internal reshape callback
	glutReshapeFunc(bopEngineReshapeCallback);

	initFlag = true;

	return true;
}

/*
This function is called for entering in freeGLUT main loop
return true if done correctly
*/
bool LIB_API bopEngine::enterMainLoop()
{
	if (!initFlag)
	{
		return false;
	}

	//Enter the main FreeGLUT processing loop
	glutMainLoop();

	return true;
}

/*
This function is called to clear the scene
- clear depth buffer
- clear color buffer
return true if done correctly
*/
bool LIB_API bopEngine::clear()
{
	if (!initFlag)
	{
		return false;
	}
	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return true;
}

/*
This function is called to free the scene
- flush the render list
- free the materials list
- free the textures list
- deinitialise FreeImage context
- delete the tree
return true if done correctly
*/
bool LIB_API bopEngine::free()
{
	if (!initFlag)
	{
		return false;
	}
	//free the materials list
	for (int i = 0; i < materials.size(); i++) {
		delete materials[i];
	}
	// free the textures list
	for (int i = 0; i < textures.size(); i++) {
		delete textures[i];
	}

	//Delete all the tree
	delete root;

	//deinitialise FreeImage context
	FreeImage_DeInitialise();
	return true;
}

/*
This function is called to register the display callback
User callback is saved in an internal display callback
return true if done correctly
*/
bool LIB_API bopEngine::registerDisplayCallback(void displayCallback(void))
{
	if (!initFlag)
	{
		return false;
	}
	userDisplayCallback = displayCallback;
	glutDisplayFunc(bopEngineDisplayCallback);
	return true;
}
/*
This is the internal displayCallback
- The buffer is cleared
- The FPS are displayed
- The userCallback is invoked
- Frames are increased
- Buffer are swapped and the scene is displayed
*/
void bopEngine::bopEngineDisplayCallback()
{
	clear();
	showFPS();
	userDisplayCallback();

	frames++;
	glutSwapBuffers();
	glutPostWindowRedisplay(windowID);
}

/*
This function is called to register a keyboard callback
User callback is saved in an internal keyboard callback
return true if done correctly
*/
bool LIB_API bopEngine::registerKeyBoardCallback(void keyboardCallback(unsigned char, int, int))
{
	if (!initFlag)
	{
		return false;
	}
	userKeyboardCallback = keyboardCallback;
	glutKeyboardFunc(bopEngineKeyboardCallback);
	return true;
}
/*
This is the interal keyboard callback.
The user callback is invoked and then the rendering is forced
*/
void bopEngine::bopEngineKeyboardCallback(unsigned char key, int mouseX, int mouseY)
{
	userKeyboardCallback(key, mouseX, mouseY);
	glutPostWindowRedisplay(windowID);
}

/*
This is the internal reshape callback.
The viewport size, the ortho and perspecitve matrix are all updated
*/
void bopEngine::bopEngineReshapeCallback(int width, int height)
{
	windowHeight = height;
	windowWidth = width;
	// Update viewport size:
	glViewport(0, 0, width, height);
	// Update matrices:
	perspective = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 1.0f, FAR_PLANE);
	ortho = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
	//Force rendering
	glutPostWindowRedisplay(windowID);
}
/*
This function is called to register a special callback
User callback is saved in an internal special callback
return true if done correctly
*/
bool LIB_API bopEngine::registerSpecialCallback(void specialCallback(int, int, int))
{
	if (!initFlag)
	{
		return false;
	}
	userSpecialCallback = specialCallback;
	glutSpecialFunc(bopEngineSpecialCallback);
	return true;
}

void bopEngine::bopEngineSpecialCallback(int key, int x, int y) {
	userSpecialCallback(key, x, y);
	glutPostWindowRedisplay(windowID);
}

/*
This function is called to register a mouse callback
User callback is registered directly to freeGlut
return true if done correctly
*/
bool LIB_API bopEngine::registerMouseCallback(void mouseCallback(int, int, int, int))
{
	if (!initFlag)
	{
		return false;
	}
	glutMouseFunc(mouseCallback);
	return true;
}
/*
This function is called to register a mouse callback
User callback is registered directly to freeGlut
return true if done correctly
*/
//TODO da testare
bool bopEngine::registerMouseMotionCallback(void mouseMotionCallback(int x, int y))
{
	if (!initFlag)
	{
		return false;
	}
	glutMotionFunc(mouseMotionCallback);
	return false;
}
void bopEngine::bopEngineTimerCallback(int val)
{
	userTimerCallback(val);
	fps = frames / 1.0f;
	frames = 0;
	//entrer? in quella del utente ogni secondo
	registerTimerCallback(userTimerCallback);
}
/*
This function is called to register a timer callback
User callback is saved in an internal timer callback
return true if done correctly
*/
bool LIB_API bopEngine::registerTimerCallback(void timerCallback(int))
{
	if (!initFlag)
	{
		return false;
	}

	userTimerCallback = timerCallback;
	glutTimerFunc(1000, bopEngineTimerCallback, 0);
	return true;
}

//Return the root
Node LIB_API *bopEngine::getRoot()
{
	return root;
}

//Return a node by his name
//delegated to the node method
Node LIB_API *bopEngine::getNodeByName(std::string name)
{
	return root->findByName(name);
}

//Return the current camera
Camera LIB_API *bopEngine::getCamera()
{
	return currentCamera;
}

//Set the current camera
void LIB_API bopEngine::setCamera(Camera * camera)
{
	currentCamera = camera;
}


/*
This function is called for rendering the scene from a list
call the list render method
*/
void bopEngine::render(List* list)
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(perspective));
	glMatrixMode(GL_MODELVIEW);
	list->render(currentCamera->getViewMatrix());
}

/*
This function is used to show fps on screen
*/
void bopEngine::showFPS() {
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(ortho));
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(glm::mat4(1.0)));
	glDisable(GL_LIGHTING);
	char text[64];
	sprintf(text, "FPS: %d", fps);
	glColor3f(0.0f, 1.0f, 0.0f);
	glRasterPos2f(1.0f, windowHeight - 20);
	glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char *)text);
	glEnable(GL_LIGHTING);
}

class OvObject
{
public:
	enum class Type : int  ///< Type of entities
	{
		// Foundation types:
		OBJECT = 0,
		NODE,
		OBJECT2D,
		OBJECT3D,
		LIST,
		// Derived classes:
		BUFFER,
		SHADER,
		TEXTURE,
		FILTER,
		MATERIAL,
		FBO,
		QUAD,
		BOX,
		SKYBOX,
		FONT,
		CAMERA,
		LIGHT,
		BONE,
		MESH,           // Keep them...
		SKINNED, // ...consecutive
		INSTANCED,
		PIPELINE,
		EMITTER,
		// Animation type
		ANIM,
		// Physics related:
		PHYSICS,
		// Terminator:
		LAST,
	};
};
// Stripped-down redefinition of OvMesh (just for the subtypes):
class OvMesh
{
public:
	enum class Subtype : int ///< Kind of mesh
	{
		// Foundation types:
		DEFAULT = 0,
		NORMALMAPPED,
		TESSELLATED,
		// Terminator:
		LAST,
	};
};
// Stripped-down redefinition of OvLight (just for the subtypes):
class OvLight
{
public:
	enum class Subtype : int ///< Kind of light
	{
		// Foundation types:
		OMNI = 0,
		DIRECTIONAL,
		SPOT,
		// Terminator:
		LAST,
	};
};

/*
This function is called for loading an .ovo file
src: is the name of the file
return a pointer to the root of the scene
*/
Node LIB_API * bopEngine::load(const char * src)
{
	// Open file:
	FILE *dat = fopen(src, "rb");
	if (dat == nullptr)
	{
		cout << "ERROR: unable to open file '" << src << "'" << endl;
		return nullptr;
	}
	unsigned int position = 0;
	// Parse chunk information according to its type:
	root = readChunk(dat, position);
	// Done:
	fclose(dat);
	cout << "\nFile parsed" << endl;

	return root;
}
/*
This function is called for building a texture
path: is the relative path of the texture
return a pointer of the texture
*/
Texture * bopEngine::buildTexture(std::string path)
{
	FIBITMAP *bitmap = FreeImage_Load(FreeImage_GetFileType(&path[0], 0), &path[0]);
	Texture* texture{ new Texture() };
	texture->setBitmap(bitmap);
	int intFormat = GL_RGB;
	GLenum extFormat = GL_BGR_EXT;
	if (FreeImage_GetBPP(bitmap) == 32)
	{
		intFormat = GL_RGBA;
		extFormat = GL_BGRA_EXT;
	}
	glBindTexture(GL_TEXTURE_2D, texture->getTextureId());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	if (bopEngine::isAnisotropicSupported)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, bopEngine::anisotropicLevel);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, intFormat, FreeImage_GetWidth(bitmap), FreeImage_GetHeight(bitmap), extFormat, GL_UNSIGNED_BYTE, (void *)FreeImage_GetBits(bitmap));
	return texture;
}
/*
This function is called recursively to read chunk and build nodes
dat: is a pointer of the .ovo file
position: is a variable used to determine how much from the chunk has been read
returns a pointer node constructed correctly by its type
*/
Node LIB_API * bopEngine::readChunk(FILE* dat, unsigned int position) {
	unsigned int chunkId, chunkSize;
	//Check if EOF
	fread(&chunkId, sizeof(unsigned int), 1, dat);
	if (feof(dat))
		return nullptr;
	fread(&chunkSize, sizeof(unsigned int), 1, dat);
	// Load whole chunk into memory:
	char *data = new char[chunkSize];
	if (fread(data, sizeof(char), chunkSize, dat) != chunkSize)
	{
		fclose(dat);
		delete[] data;
		return nullptr;
	}

	position = 0;
	//From the chunk id we know the type of the chunk to read
	switch ((OvObject::Type) chunkId)
	{
		/*
		chunk id = 0
		chunk type = material
		From this chunk we get:
		- version of the ovoreader
		*/
	case OvObject::Type::OBJECT:
	{
		cout << "version]" << endl;
		// OVO revision number:
		unsigned int versionId;
		memcpy(&versionId, data + position, sizeof(unsigned int));
		cout << "   Version . . . :  " << versionId << endl;
		position += sizeof(unsigned int);
		delete[] data;
		return readChunk(dat, position);
	}
	/*
	chunk id = 1
	chunk type = node
	From this chunk we get:
	- name
	- nr. of childrean
	- target node ([none] if not used)
	- position's matrix
	*/
	case OvObject::Type::NODE:
	{
		cout << "node]" << endl;
		Node* node{ new Node() };
		// Node name:
		char nodeName[FILENAME_MAX];
		strcpy(nodeName, data + position);
		node->setName(nodeName);
		cout << "   Name  . . . . :  " << nodeName << endl;
		position += (unsigned int)strlen(nodeName) + 1;
		// Node matrix:
		glm::mat4 matrix;
		memcpy(&matrix, data + position, sizeof(glm::mat4));
		node->setPosition(matrix);
		position += sizeof(glm::mat4);
		// Nr. of children nodes:
		unsigned int children;
		memcpy(&children, data + position, sizeof(unsigned int));
		cout << "   Nr. children  :  " << children << endl;
		position += sizeof(unsigned int);
		// Optional target node, [none] if not used:
		char targetName[FILENAME_MAX];
		strcpy(targetName, data + position);
		cout << "   Target node . :  " << targetName << endl;
		position += (unsigned int)strlen(targetName) + 1;
		// If the node has children the method readChunk is invoked recursively for each child
		// Each child returned is linked to the father (node)
		if (children) {
			while (node->getNrOfChildren() < children) {
				node->link(readChunk(dat, position));
			}
		}
		delete[] data;
		return node;
	}
	/*
	chunk id = 9
	chunk type = material
	From this chunk we get:
	- name of material
	- emissione values
	- albedo values
	- roughness value
	- transparency value (1 if is solid)
	- texture name
	- normalmap tex. ([none] if not used)
	- heightmap tex. ([none] if not used)
	- roughness tex. ([none] if not used)
	- metalness tex. ([none] if not used)
	*/
	case OvObject::Type::MATERIAL:
	{
		Material* material{ new Material() };
		// Material name:
		char materialName[FILENAME_MAX];
		strcpy(materialName, data + position);
		cout << "   Name  . . . . :  " << materialName << endl;
		position += (unsigned int)strlen(materialName) + 1;
		material->setName(materialName);
		// Material term colors, starting with emissive:
		glm::vec3 emission, albedo;
		memcpy(&emission, data + position, sizeof(glm::vec3));
		cout << "   Emission  . . :  " << emission.r << ", " << emission.g << ", " << emission.b << endl;
		position += sizeof(glm::vec3);
		// Albedo:
		memcpy(&albedo, data + position, sizeof(glm::vec3));
		cout << "   Albedo  . . . :  " << albedo.r << ", " << albedo.g << ", " << albedo.b << endl;
		position += sizeof(glm::vec3);
		material->setProperties(albedo);
		// Roughness factor:
		float roughness;
		memcpy(&roughness, data + position, sizeof(float));
		cout << "   Roughness . . :  " << roughness << endl;
		position += sizeof(float);
		material->setShininess(roughness);
		// Metalness factor:
		float metalness;
		memcpy(&metalness, data + position, sizeof(float));
		cout << "   Metalness . . :  " << metalness << endl;
		position += sizeof(float);
		// Transparency factor:
		float alpha;
		memcpy(&alpha, data + position, sizeof(float));
		cout << "   Transparency  :  " << alpha << endl;
		position += sizeof(float);
		if (alpha > 1) {
			alpha = 1;
		}
		material->setTransparency(alpha);

		// Albedo texture filename, or [none] if not used:
		char textureName[FILENAME_MAX];
		strcpy(textureName, data + position);
		cout << "   Albedo tex. . :  " << textureName << endl;
		position += (unsigned int)strlen(textureName) + 1;
		bool textureFound = false;
		for (Texture* tex : textures) {
			if (tex->getName() == textureName) {
				material->setTexture(tex);
				textureFound = true;
				break;
			}
		}
		//If the texture isn't loaded we should load it
		if (!textureFound) {
			char full_texture_path[256];
			strcpy(full_texture_path, resourcesPath);
			strcat(full_texture_path, textureName);
			//Load and build the texture
			Texture* texture = buildTexture(full_texture_path);
			texture->setName(textureName);
			textures.push_back(texture);
			material->setTexture(texture);
		}
		// Normal map filename, or [none] if not used:
		char normalMapName[FILENAME_MAX];
		strcpy(normalMapName, data + position);
		cout << "   Normalmap tex.:  " << normalMapName << endl;
		position += (unsigned int)strlen(normalMapName) + 1;
		// Height map filename, or [none] if not used:
		char heightMapName[FILENAME_MAX];
		strcpy(heightMapName, data + position);
		cout << "   Heightmap tex.:  " << heightMapName << endl;
		position += (unsigned int)strlen(heightMapName) + 1;
		// Roughness map filename, or [none] if not used:
		char roughnessMapName[FILENAME_MAX];
		strcpy(roughnessMapName, data + position);
		cout << "   Roughness tex.:  " << roughnessMapName << endl;
		position += (unsigned int)strlen(roughnessMapName) + 1;
		// Metalness map filename, or [none] if not used:
		char metalnessMapName[FILENAME_MAX];
		strcpy(metalnessMapName, data + position);
		cout << "   Metalness tex.:  " << metalnessMapName << endl;
		position += (unsigned int)strlen(metalnessMapName) + 1;
		delete[] data;

		//Material is added to the list of materials
		materials.push_back(material);
		return readChunk(dat, position);
	}

	/*
	chunk id = 18
	chunk type = mesh
	chunk id = 19
	chunk type = skinned
	From this chunk we get:
	- name of mesh/skinned
	- nr. of children
	- target node ([none] if not used)
	- subtype (default = 0, normalmapped = 1, tessellated = 2, last = 3)
	- nr. of vertices
	- nr. of faces
	- name of material
	- mesh/skinned radiius size
	- mesh/skinned bounding box minimum corner
	- mesh/skinned bounding box maximum corner
	- physics (0 = no, 1 = yes)
	*/
	case OvObject::Type::MESH:
	case OvObject::Type::SKINNED:
	{
		// Both standard and skinned meshes are handled through this case:
		bool isSkinned = false;
		if ((OvObject::Type) chunkId == OvObject::Type::SKINNED)
		{
			isSkinned = true;
			cout << "skinned mesh]" << endl;
		}
		else
			cout << "mesh]" << endl;
		Mesh* mesh{ new Mesh() };
		// Mesh name:
		char meshName[FILENAME_MAX];
		strcpy(meshName, data + position);
		position += (unsigned int)strlen(meshName) + 1;
		cout << "   Name  . . . . :  " << meshName << endl;
		mesh->setName(meshName);
		// Mesh matrix:
		glm::mat4 matrix;
		memcpy(&matrix, data + position, sizeof(glm::mat4));
		position += sizeof(glm::mat4);
		mesh->setPosition(matrix);
		// Mesh nr. of children nodes:
		unsigned int children;
		memcpy(&children, data + position, sizeof(unsigned int));
		cout << "   Nr. children  :  " << children << endl;
		position += sizeof(unsigned int);
		// Optional target node, or [none] if not used:
		char targetName[FILENAME_MAX];
		strcpy(targetName, data + position);
		cout << "   Target node . :  " << targetName << endl;
		position += (unsigned int)strlen(targetName) + 1;
		// Mesh subtype (see OvMesh SUBTYPE enum):
		unsigned char subtype;
		memcpy(&subtype, data + position, sizeof(unsigned char));
		char subtypeName[FILENAME_MAX];
		switch ((OvMesh::Subtype) subtype)
		{
		case OvMesh::Subtype::DEFAULT: strcpy(subtypeName, "standard"); break;
		case OvMesh::Subtype::NORMALMAPPED: strcpy(subtypeName, "normal-mapped"); break;
		case OvMesh::Subtype::TESSELLATED: strcpy(subtypeName, "tessellated"); break;
		default: strcpy(subtypeName, "UNDEFINED");
		}
		cout << "   Subtype . . . :  " << (int)subtype << " (" << subtypeName << ")" << endl;
		position += sizeof(unsigned char);
		// Nr. of vertices:
		unsigned int vertices, faces;
		memcpy(&vertices, data + position, sizeof(unsigned int));
		cout << "   Nr. vertices  :  " << vertices << endl;
		position += sizeof(unsigned int);
		// ...and faces:
		memcpy(&faces, data + position, sizeof(unsigned int));
		cout << "   Nr. faces . . :  " << faces << endl;
		position += sizeof(unsigned int);
		// Material name, or [none] if not used:
		char materialName[FILENAME_MAX];
		strcpy(materialName, data + position);
		cout << "   Material  . . :  " << materialName << endl;
		position += (unsigned int)strlen(materialName) + 1;

		for (int i = 0; i < materials.size(); i++) {
			if (materials[i]->getName() == materialName) {
				mesh->setMaterial(materials[i]);
				break;
			}
		}

		// Mesh bounding sphere radius:
		float radius;
		memcpy(&radius, data + position, sizeof(float));
		cout << "   Radius  . . . :  " << radius << endl;
		position += sizeof(float);
		// Mesh bounding box minimum corner:
		glm::vec3 bBoxMin;
		memcpy(&bBoxMin, data + position, sizeof(glm::vec3));
		cout << "   BBox minimum  :  " << bBoxMin.x << ", " << bBoxMin.y << ", " << bBoxMin.z << endl;
		position += sizeof(glm::vec3);
		// Mesh bounding box maximum corner:
		glm::vec3 bBoxMax;
		memcpy(&bBoxMax, data + position, sizeof(glm::vec3));
		cout << "   BBox maximum  :  " << bBoxMax.x << ", " << bBoxMax.y << ", " << bBoxMax.z << endl;
		position += sizeof(glm::vec3);
		// Optional physics properties:
		unsigned char hasPhysics;
		memcpy(&hasPhysics, data + position, sizeof(unsigned char));
		cout << "   Physics . . . :  " << (int)hasPhysics << endl;
		position += sizeof(unsigned char);
		if (hasPhysics)
		{
			/**
			 * Mesh physics properties.
			 */
			struct PhysProps
			{
				// Pay attention to 16 byte alignement (use padding):
				unsigned char type;
				unsigned char contCollisionDetection;
				unsigned char collideWithRBodies;
				unsigned char hullType;
				// Vector data:
				glm::vec3 massCenter;
				// Mesh properties:
				float mass;
				float staticFriction;
				float dynamicFriction;
				float bounciness;
				float linearDamping;
				float angularDamping;
				void *physObj;
			};
			PhysProps mp;
			memcpy(&mp, data + position, sizeof(PhysProps));
			position += sizeof(PhysProps);
		}
		std::vector<Vertex*> tmpVertex;
		// Interleaved and compressed vertex/normal/UV/tangent data:
		for (unsigned int c = 0; c < vertices; c++)
		{
			Vertex* v{ new Vertex() };
			// Vertex coords:
			glm::vec3 vertex;
			memcpy(&vertex, data + position, sizeof(glm::vec3));
			position += sizeof(glm::vec3);
			v->setCoordinates(vertex);
			// Vertex normal:
			unsigned int normalData;
			memcpy(&normalData, data + position, sizeof(unsigned int));
			position += sizeof(unsigned int);
			glm::vec4 normal = glm::unpackSnorm3x10_1x2(normalData);
			v->setNormal(normal);
			// Texture coordinates:
			unsigned short textureData[2];
			memcpy(textureData, data + position, sizeof(unsigned short) * 2);
			position += sizeof(unsigned short) * 2;
			glm::vec2 uv;
			uv.x = glm::unpackHalf1x16(textureData[0]);
			uv.y = glm::unpackHalf1x16(textureData[1]);
			v->setTextureCoordinates(uv);
			// Tangent vector:
			unsigned int tangentData;
			memcpy(&tangentData, data + position, sizeof(unsigned int));
			position += sizeof(unsigned int);

			tmpVertex.push_back(v);
		}
		// Faces:
		for (unsigned int c = 0; c < faces; c++)
		{
			// Face indexes:
			unsigned int face[3];
			memcpy(face, data + position, sizeof(unsigned int) * 3);
			position += sizeof(unsigned int) * 3;
			Face* f{ new Face() };
			f->setFace(tmpVertex[face[0]], tmpVertex[face[1]], tmpVertex[face[2]]);
			mesh->addFace(f);
		}
		// Extra information for skinned meshes:
		if (isSkinned)
		{
			// Initial mesh pose matrix:
			glm::mat4 poseMatrix;
			memcpy(&poseMatrix, data + position, sizeof(glm::mat4));
			position += sizeof(glm::vec4);
			// Bone list:
			unsigned int nrOfBones;
			memcpy(&nrOfBones, data + position, sizeof(unsigned int));
			cout << "   Nr. bones . . :  " << nrOfBones << endl;
			position += sizeof(unsigned int);
			for (unsigned int c = 0; c < nrOfBones; c++)
			{
				// Bone name:
				char boneName[FILENAME_MAX];
				strcpy(boneName, data + position);
				cout << "      Bone name  :  " << boneName << " (" << c << ")" << endl;
				position += (unsigned int)strlen(boneName) + 1;
				// Initial bone pose matrix (already inverted):
				glm::mat4 boneMatrix;
				memcpy(&boneMatrix, data + position, sizeof(glm::mat4));
				position += sizeof(glm::mat4);
			}
			// Per vertex bone weights and indexes:
			for (unsigned int c = 0; c < vertices; c++)
			{

				// Bone indexes:
				unsigned int boneIndex[4];
				memcpy(boneIndex, data + position, sizeof(unsigned int) * 4);
				position += sizeof(unsigned int) * 4;
				// Bone weights:
				unsigned short boneWeightData[4];
				memcpy(boneWeightData, data + position, sizeof(unsigned short) * 4);
				position += sizeof(unsigned short) * 4;
			}
		}

		if (children) {
			while (mesh->getNrOfChildren() < children) {
				mesh->link(readChunk(dat, position));
			}
		}
		delete[] data;
		return mesh;
	}
	/*
	chunk id = 16
	chunk type = light
	From this chunk we get:
	- name of light
	- nr. of children
	- target node ([none] if not used)
	- subtype (default = 0, normalmapped = 1, tessellated = 2, last = 3)
	- color values
	- radius value
	- direction values
	- cutoff vale
	- spot exponent value
	- cast shadows value
	- volumetric value
	*/
	case OvObject::Type::LIGHT:
	{
		Light* light{ new Light() };
		cout << "light]" << endl;
		// Light name:
		char lightName[FILENAME_MAX];
		strcpy(lightName, data + position);
		cout << "   Name  . . . . :  " << lightName << endl;
		position += (unsigned int)strlen(lightName) + 1;
		light->setName(lightName);
		// Light matrix:
		glm::mat4 matrix;
		memcpy(&matrix, data + position, sizeof(glm::mat4));
		position += sizeof(glm::mat4);

		// Nr. of children nodes:
		unsigned int children;
		memcpy(&children, data + position, sizeof(unsigned int));
		cout << "   Nr. children  :  " << children << endl;
		position += sizeof(unsigned int);

		// Optional target node name, or [none] if not used:
		char targetName[FILENAME_MAX];
		strcpy(targetName, data + position);
		cout << "   Target node . :  " << targetName << endl;
		position += (unsigned int)strlen(targetName) + 1;

		// Light subtype (see OvLight SUBTYPE enum):
		unsigned char subtype;
		memcpy(&subtype, data + position, sizeof(unsigned char));
		char subtypeName[FILENAME_MAX];
		switch ((OvLight::Subtype) subtype)
		{
			case OvLight::Subtype::DIRECTIONAL: strcpy(subtypeName, "directional"); break;
			case OvLight::Subtype::OMNI: strcpy(subtypeName, "omni"); break;
			case OvLight::Subtype::SPOT: strcpy(subtypeName, "spot"); break;
			default: strcpy(subtypeName, "UNDEFINED");
		}
		cout << "   Subtype . . . :  " << (int)subtype << " (" << subtypeName << ")" << endl;
		position += sizeof(unsigned char);

		light->setLightType((LIGHT_TYPE)(int)subtype);

		light->setPosition(matrix);

		// Light color:
		glm::vec3 color;
		memcpy(&color, data + position, sizeof(glm::vec3));
		cout << "   Color . . . . :  " << color.r << ", " << color.g << ", " << color.b << endl;
		position += sizeof(glm::vec3);
		light->setColor(color);

		// Influence radius:
		float radius;
		memcpy(&radius, data + position, sizeof(float));
		cout << "   Radius  . . . :  " << radius << endl;
		position += sizeof(float);
		light->setRadius(radius);

		// Direction:
		glm::vec3 direction;
		memcpy(&direction, data + position, sizeof(glm::vec3));
		cout << "   Direction . . :  " << direction.r << ", " << direction.g << ", " << direction.b << endl;
		position += sizeof(glm::vec3);
		light->setDirection(direction);

		// Cutoff:
		float cutoff;
		memcpy(&cutoff, data + position, sizeof(float));
		cout << "   Cutoff  . . . :  " << cutoff << endl;
		position += sizeof(float);
		light->setCutoff(cutoff);

		// Exponent:
		float spotExponent;
		memcpy(&spotExponent, data + position, sizeof(float));
		cout << "   Spot exponent :  " << spotExponent << endl;
		position += sizeof(float);

		// Cast shadow flag:
		unsigned char castShadows;
		memcpy(&castShadows, data + position, sizeof(unsigned char));
		cout << "   Cast shadows  :  " << (int)castShadows << endl;
		position += sizeof(unsigned char);

		// Volumetric lighting flag:
		unsigned char isVolumetric;
		memcpy(&isVolumetric, data + position, sizeof(unsigned char));
		cout << "   Volumetric  . :  " << (int)isVolumetric << endl;
		position += sizeof(unsigned char);
		if (children) {
			while (light->getNrOfChildren() < children) {
				light->link(readChunk(dat, position));
			}
		}

		delete[] data;
		return light;
	}
	break;
	/*
	chunk id = 18
	chunk type = bone
	From this chunk we get:
	- name of bone
	- position of bone
	- nr. of children
	- target node ([none] if not used)
	- mesh bounding box minimum corner
	- mesh bounding box maximum corne
	*/
	case OvObject::Type::BONE:
	{
		cout << "bone]" << endl;
		// Bone name:
		char boneName[FILENAME_MAX];
		strcpy(boneName, data + position);
		cout << "   Name  . . . . :  " << boneName << endl;
		position += (unsigned int)strlen(boneName) + 1;
		// Bone matrix:
		glm::mat4 matrix;
		memcpy(&matrix, data + position, sizeof(glm::mat4));
		position += sizeof(glm::mat4);
		// Nr. of children nodes:
		unsigned int children;
		memcpy(&children, data + position, sizeof(unsigned int));
		cout << "   Nr. children  :  " << children << endl;
		position += sizeof(unsigned int);
		// Optional target node, or [none] if not used:
		char targetName[FILENAME_MAX];
		strcpy(targetName, data + position);
		cout << "   Target node . :  " << targetName << endl;
		position += (unsigned int)strlen(targetName) + 1;
		// Mesh bounding box minimum corner:
		glm::vec3 bBoxMin;
		memcpy(&bBoxMin, data + position, sizeof(glm::vec3));
		cout << "   BBox minimum  :  " << bBoxMin.x << ", " << bBoxMin.y << ", " << bBoxMin.z << endl;
		position += sizeof(glm::vec3);
		// Mesh bounding box maximum corner:
		glm::vec3 bBoxMax;
		memcpy(&bBoxMax, data + position, sizeof(glm::vec3));
		cout << "   BBox maximum  :  " << bBoxMax.x << ", " << bBoxMax.y << ", " << bBoxMax.z << endl;
		position += sizeof(glm::vec3);
		for (int i = 0; i < children; i++) {
			readChunk(dat, position);
		}
		delete[] data;
		return readChunk(dat, position);
	}
	default:
		cout << "UNKNOWN]" << endl;
		cout << "ERROR: corrupted or bad data in file " << endl;
		fclose(dat);
		delete[] data;
		return nullptr;
	}
	// Release chunk memory:
	delete[] data;
}
