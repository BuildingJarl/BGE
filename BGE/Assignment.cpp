#include "Assignment.h"
#include "PhysicsController.h"
#include <btBulletDynamicsCommon.h>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <gtx/euler_angles.hpp>
#include <gtx/norm.hpp>
#include "VectorDrawer.h"
#include "Content.h"
#include "Box.h"
#include "Cylinder.h"
#include "PhysicsCamera.h"
#include "Sphere.h"
#include <btBulletDynamicsCommon.h>
#include "Utils.h"
#include "Hand.h"
#include "LineHand.h"

using namespace BGE;

Assignment::Assignment(void)
{
	physicsFactory = NULL;
	dynamicsWorld = NULL;
	broadphase = NULL;
	dispatcher = NULL;
	solver = NULL;
	fullscreen = false;
}

Assignment::~Assignment(void)
{
}


bool Assignment::Initialise()
{
	//settings
	riftEnabled = false;
	fullscreen = false;

	//leapMotion
	leapmotionController.addListener(leapmotionListener);

	//Physics Engine
	// Set up the collision configuration and dispatcher
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    
	// The world.
	btVector3 worldMin(-1000,-1000,-1000);
	btVector3 worldMax(1000,1000,1000);
	broadphase = new btAxisSweep3(worldMin,worldMax);
	solver = new btSequentialImpulseConstraintSolver();
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0,-9,0));
	
	//Physics Factory
	physicsFactory = make_shared<PhysicsFactory>(dynamicsWorld);
	
	physicsFactory->CreateGroundPhysics();
	physicsFactory->CreateCameraPhysics();
	physicsFactory->CreateWall(glm::vec3(-20,0,-100), 5, 5, 50,50,50);

	shared_ptr<Hand> hand1 = make_shared<Hand>(leapmotionController, dynamicsWorld);
	Attach(hand1);

	//shared_ptr<LineHand> hand2 = make_shared<LineHand>(leapmotionController, dynamicsWorld);
	//Attach(hand2);
	
	//Init Game
	Game::Initialise();
	camera->GetController()->position = glm::vec3(0, 100, 220);
	return true;
}

void Assignment::Update(float timeDelta)
{	

	if(leapmotionListener.debugInfo.size() > 0)
	{
		Game::PrintText(leapmotionListener.debugInfo);
	}

	dynamicsWorld->stepSimulation(timeDelta,100);
	Game::Update(timeDelta);
}

void Assignment::Cleanup()
{

	leapmotionController.removeListener(leapmotionListener);
	
	Game::Cleanup();
}