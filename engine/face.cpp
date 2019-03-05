#include "bopEngine.h"
#include "GL/freeglut.h"


Face::Face()
{
}


Face::~Face()
{
}

void Face::render(glm::mat4 view)
{
	glm::vec3 v1, v2, v3;
	v1 = vert1->getCoordinates();
	v2 = vert2->getCoordinates();
	v3 = vert3->getCoordinates();
	
	glBegin(GL_TRIANGLES);

	glNormal3f(vert1->getNormal().x, vert1->getNormal().y, vert1->getNormal().z);
	glTexCoord2f(vert1->getTextureCoordinates().x, vert1->getTextureCoordinates().y);
	glVertex3f(v1.x, v1.y, v1.z);


	glNormal3f(vert2->getNormal().x, vert2->getNormal().y, vert2->getNormal().z);
	glTexCoord2f(vert2->getTextureCoordinates().x, vert2->getTextureCoordinates().y);
	glVertex3f(v2.x, v2.y, v2.z);
	

	glNormal3f(vert3->getNormal().x, vert3->getNormal().y, vert3->getNormal().z);
	glTexCoord2f(vert3->getTextureCoordinates().x, vert3->getTextureCoordinates().y);
	glVertex3f(v3.x,v3.y,v3.z);
		
	glEnd();
}
