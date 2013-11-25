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

	physicsFactory->CreateWall(glm::vec3(-60,0,-50), 20, 5);

	//--------------------------------------
	float mass = 10;
	float radius = 1;
	btVector3 inertia(0,0,0);
	
	//create container for sphere (what wee see on screen)
	shared_ptr<GameComponent> comp (new Sphere(radius));
	leapMotionHand = comp;
	leapMotionHand->Attach(Content::LoadModel("sphere", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0,1,0))));
	leapMotionHand->position = glm::vec3(0,10,-30);
	Attach(leapMotionHand); // attach to game

	//create collisionShape for sphere
	btCollisionShape * palmColShape = new btSphereShape(radius);
	palmColShape->calculateLocalInertia(mass,inertia);

	//Create PhysicsHand component and attach to sphere
	shared_ptr<PhysicsHand> physicsHand = make_shared<PhysicsHand>();
	leapMotionHand->Attach(physicsHand);

	//create a rigid body
	btRigidBody::btRigidBodyConstructionInfo palmCI(mass,physicsHand.get(),palmColShape,inertia);
	btRigidBody * body = new btRigidBody(palmCI);
	physicsHand->SetPhysicsStuff(palmColShape,body,physicsHand.get());
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	body->setActivationState(DISABLE_DEACTIVATION);
	
	//attach to world
	dynamicsWorld->addRigidBody(body);
	//--------------------------------------

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

			shared_ptr<Camera> camera = Game::Instance()->camera;

			if(z > camera->position.z)
			{
				z = camera->position.z - 10;
			}

			leapMotionHand->position = glm::vec3(x,y,z) ;
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