#ifndef enemy2H
#define enemy2H

#include "PhysicObject2.h"

class MEnemy2: public MPhysicObject2
{
private:
	MPhysicObject2* pTarget;
	glm::vec2 MainVelocity;
	glm::vec2 Direction;
	float Rotation;
public:
	MEnemy2();
	~MEnemy2();
	void Follow();
	void SetTarget(MPhysicObject2* inpTarget);
};

#endif


