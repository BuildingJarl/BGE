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
#include "PhysicsHand.h"
#include "Hand.h"

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
	leapEnabled = true;

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
	physicsFactory = make_shared<PhysicsFactory>(dynamicsWorld);
	physicsFactory->CreateGroundPhysics();
	//physicsFactory->CreateCameraPhysics();

	physicsFactory->CreateWall(glm::vec3(-20,0,-50), 5, 5);

	shared_ptr<Hand> hand = make_shared<Hand>(leapmotionController,dynamicsWorld);
	Attach(hand);
	
	//Init Game
	Game::Initialise();

	//set camera to be above world
	camera->GetController()->position = glm::vec3(5, 4, 20);

	return true;
}

void Assignment::Update(float timeDelta)
{
		
	dynamicsWorld->stepSimulation(timeDelta,100);

	Game::Update(timeDelta);
}

void Assignment::Cleanup()
{
	Game::Cleanup();
}