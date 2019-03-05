#pragma once

class LIB_API Object
{
public:
	Object();
	virtual ~Object();

	virtual void render(glm::mat4 view) = 0; //pure virtual
		
	inline void setName(const std::string &name) { this->name = name;  };
	
	inline int getId() { return id; };
	inline std::string getName() { return name; };
	
private:
	unsigned int id;
	std::string name;
	static unsigned int currentId;
};
