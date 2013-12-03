#include "Hand.h"
#include "Camera.h"
#include "PhysicsHand.h"
#include "Content.h"
#include "Sphere.h"

using namespace BGE;

Hand::Hand(Leap::Controller leapmotionController, btDiscreteDynamicsWorld * dynamicsWorld)
{
	
	this->leapmotionController = leapmotionController;
	this->dynamicsWorld = dynamicsWorld;

	CreatePalm();
	CreateFingers();

	//create 5 spheres with kinematic rigid bodies and add them to the vector
	//fingers.push_back();



}


Hand::~Hand(void)
{
}

void Hand::Update(float timeDelta) 
{

	if(leapmotionController.isConnected())
	{
		Leap::Frame f = leapmotionController.frame();
		Leap::HandList handList  = f.hands();

		if(handList.count() > 0)
		{
			Leap::Hand hand = handList[0];

			float x = hand.palmPosition().x;
			float y = hand.palmPosition().y - 200;
			float z = hand.palmPosition().z;

			//not sure about camera here if hand is connected to camera through some joint (cone joint) then the below code isnt needed!
			shared_ptr<Camera> camera = Game::Instance()->camera;

			if(z > camera->position.z)
			{
				z = camera->position.z - 50;
			}

			//translate to position with use of function from lab 4 or 5 -> might make everything smoother!
			palm->position = glm::vec3(x,y,z) ;

			Leap::FingerList fingerList = hand.fingers();

			for(int f = 0; f < fingerList.count(); f++) {
				Leap::Finger finger = fingerList[f];
				Leap::Vector pos = finger.tipPosition(); //i think the position is in mm?

				//fingers[f]->position = pos;
			};

		}
	}
	
	GameComponent::Update(timeDelta);
}

void Hand::CreatePalm()
{
	float mass = 10;
	float radius = 3;
	btVector3 inertia(0,0,0);
	
	//create container for palm (what wee see on screen)
	palm = shared_ptr<GameComponent> (new Sphere(radius));;
	palm->Attach(Content::LoadModel("sphere", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0,1,0))));
	palm->position = glm::vec3(0,10,-30);
	Attach(palm); // attach to game

	//create collisionShape for sphere
	btCollisionShape * palmColShape = new btSphereShape(radius);
	palmColShape->calculateLocalInertia(mass,inertia);

	//Create PhysicsHand component and attach to sphere
	shared_ptr<PhysicsHand> physicsHand = make_shared<PhysicsHand>();
	palm->Attach(physicsHand);

	//create a rigid body
	btRigidBody::btRigidBodyConstructionInfo palmCI(mass,physicsHand.get(),palmColShape,inertia);
	btRigidBody * body = new btRigidBody(palmCI);
	physicsHand->SetPhysicsStuff(palmColShape,body,physicsHand.get());
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	body->setActivationState(DISABLE_DEACTIVATION);
	
	//attach to world
	this->dynamicsWorld->addRigidBody(body);
}

void Hand::CreateFingers()
{
	for(int fingerCount = 0 ; fingerCount < 5 ; fingerCount ++)
	{
		float mass = 5;
		float radius = 1;
		btVector3 inertia(0,0,0);
	
		//create container for palm (what wee see on screen)
		shared_ptr<GameComponent> fingerTip = shared_ptr<GameComponent> (new Sphere(radius));;
		fingerTip->Attach(Content::LoadModel("sphere", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0,1,0))));
		fingerTip->position = glm::vec3((palm->position.x - 3 ) + (fingerCount * 5),palm->position.y, palm->position.z - (18 + fingerCount));
		Attach(fingerTip); // attach to game

		//create collisionShape for sphere
		btCollisionShape * fingerTipColShape = new btSphereShape(radius);
		fingerTipColShape->calculateLocalInertia(mass,inertia);

		//Create PhysicsHand component and attach to sphere
		shared_ptr<PhysicsHand> physicsHand = make_shared<PhysicsHand>();
		fingerTip->Attach(physicsHand);

		//create a rigid body
		btRigidBody::btRigidBodyConstructionInfo fingerTipCI(mass,physicsHand.get(),fingerTipColShape,inertia);
		btRigidBody * body = new btRigidBody(fingerTipCI);
		physicsHand->SetPhysicsStuff(fingerTipColShape,body,physicsHand.get());
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		body->setActivationState(DISABLE_DEACTIVATION);
	
		//add joint palm object!!!	

		//attach to world
		this->dynamicsWorld->addRigidBody(body);
	}
}