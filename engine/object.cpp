#include "bopEngine.h"

unsigned int Object::currentId = 0;


LIB_API Object::Object()
{
	this->id = currentId;
	currentId++;
}


LIB_API Object::~Object()
{

}

