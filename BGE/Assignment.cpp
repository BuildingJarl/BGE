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
#include "FPSLeapController.h"
#include "FountainEffect.h"

using namespace BGE;

Assignment::Assignment(void)
{
	width = 1000;
	height = 800;
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

	/* Assignment Submissions */

	//Leap Motion Hands

	//1) Leap Motions hand displayed in BGE
	//Without Joints
	//shared_ptr<Hand> hand1 = make_shared<Hand>(leapmotionController, dynamicsWorld);
	//Attach(hand1);

	//With joints
	//shared_ptr<LineHand> hand2 = make_shared<LineHand>(leapmotionController, dynamicsWorld);
	//Attach(hand2);

	//2) Leap Motion FPS Controller + Gravity Gun
	shared_ptr<FPSLeapController> FPSLeapcontroller = make_shared<FPSLeapController>(leapmotionController, physicsFactory);
	camera->Attach(FPSLeapcontroller);

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
	//Important -> Remove listener
	leapmotionController.removeListener(leapmotionListener);
	
	Game::Cleanup();
}