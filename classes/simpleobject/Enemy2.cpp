#include "../../stdafx.h"
#include "Enemy2.h"

MEnemy2::MEnemy2():MPhysicObject2()
{
	pTarget = NULL;
	MainVelocity = glm::vec2(0.004, 0.004);
	Rotation = 0;
}

MEnemy2::~MEnemy2()
{
	pTarget = NULL;
	MainVelocity = glm::vec2(0.04, 0.04);
	Rotation = 0;
}

void MEnemy2::Follow()
{
	if(!pTarget)
	{
		Velocity = glm::vec2(0, 0);
		return;
	}
	Direction = glm::normalize(GetQuadCenter(pTarget->GetCollisionBox()) - GetQuadCenter(CollisionBox));
	Rotation = atan2(Direction.y, Direction.x);
	SetVertex(QuadRotate(CollisionBox, Rotation));
	Velocity = Direction * MainVelocity;
}

void MEnemy2::SetTarget(MPhysicObject2* inpTarget)
{
	pTarget = inpTarget;
}
