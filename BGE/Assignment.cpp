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
#include "LeapHandGun.h"
#include "FPSLeapController.h"
#include "FountainEffect.h"

using namespace BGE;

Assignment::Assignment(void)
{
	width = 1200;
	height = 1000;
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


	//Add LeapMotion Listener to Leap Controller 
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
	physicsFactory = new PhysicsFactory(dynamicsWorld);
	
	physicsFactory->CreateGroundPhysics();
	physicsFactory->CreateCameraPhysics();

	physicsFactory->CreateWall(glm::vec3(-20,0,-100), 5, 5, 20,10,20);
	physicsFactory->CreateWall(glm::vec3(-80,0,-100), 5, 5, 10,10,20);

	/* Assignment Submissions */

	//Leap Motion Hands

	shared_ptr<Hand> leapHand = make_shared<Hand>(leapmotionController,physicsFactory);
	Attach(leapHand);

	//shared_ptr<LeapHandGun> leapHandGun = make_shared<LeapHandGun>(leapmotionController,physicsFactory);
	//Attach(leapHandGun);

	//shared_ptr<LineHand> lineHand = make_shared<LineHand>(leapmotionController,dynamicsWorld);
	//Attach(lineHand);

	//Init Game
	Game::Initialise();
	return true;
}

void Assignment::Update(float timeDelta)
{	
	Game::PrintText("Press Tab to change guns");
	if(leapmotionListener.debugInfo.size() > 0)
	{
		Game::PrintText(leapmotionListener.debugInfo);
	}

	dynamicsWorld->stepSimulation(timeDelta,100);
	Game::Update(timeDelta);
}

void Assignment::Cleanup()
{
	//Important -> Remove listener
	leapmotionController.removeListener(leapmotionListener);
	
	Game::Cleanup();
}