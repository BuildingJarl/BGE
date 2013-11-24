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

	physicsFactory->CreateWall(glm::vec3(-60,0,-100), 20, 5);

	//--------------------------------------

	//--------------------------------------

	//create hand
	//attatch leapHandController to hand (calculates position in its own update method)

	//Init Game
	Game::Initialise();

	//set camera to be above world
	camera->GetController()->position = glm::vec3(5, 4, 20);

	return true;
}

void Assignment::Update(float timeDelta)
{
	//----------------------------------------------
	if(leapmotionController.isConnected())
	{
		Leap::Frame f = leapmotionController.frame();
		//Leap::Frame fprevious = leapmotionController.frame(1);

		Leap::HandList handList  = f.hands();

		if(handList.count() > 0)
		{
			Leap::Hand hand = handList[0];

			float x = hand.palmPosition().x;
			float y = hand.palmPosition().y - 200;
			float z = hand.palmPosition().z;

		}
	}
	
	//----------------------------------------------


	
	dynamicsWorld->stepSimulation(timeDelta,100);

	Game::Update(timeDelta);
}

void Assignment::Cleanup()
{
	Game::Cleanup();
}