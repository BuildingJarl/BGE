#include "KinematicController.h"
#include "Utils.h"

using namespace BGE;

KinematicController::KinematicController():PhysicsController()
{
	tag = "Kinematic Controller";
}


KinematicController::~KinematicController(void)
{
}

void KinematicController::getWorldTransform(btTransform &worldTrans) const
{
	worldTrans.setOrigin(GLToBtVector(parent->position));
	worldTrans.setRotation(GLToBtQuat(parent->orientation));
}

void KinematicController::setWorldTransform(const btTransform &worldTrans)
{
	// Should never get called, because this body is kinematic??
}

void KinematicController::Update(float timeDelta)
{
	// Override the one in the base class, we do not want to update our world transform from the physics object
	// WHich is what the one in the base class does...
}

