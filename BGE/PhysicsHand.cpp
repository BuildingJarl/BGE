#include "PhysicsHand.h"

using namespace BGE;

PhysicsHand::PhysicsHand()
{
	//pass in Leapmotion controller Here??

	tag = "Physics Hand";
}


PhysicsHand::~PhysicsHand(void)
{
}



void PhysicsHand::getWorldTransform(btTransform &worldTrans) const
{
	worldTrans.setOrigin(GLToBtVector(parent->position));
	worldTrans.setRotation(GLToBtQuat(parent->orientation));
}

void PhysicsHand::setWorldTransform(const btTransform &worldTrans)
{
	// Should never get called, because this body is kinematic??
}

void PhysicsHand::Update(float timeDelta)
{
	// Override the one in the base class, we do not want to update our world transform from the physics object
	// WHich is what the one in the base class does...
}