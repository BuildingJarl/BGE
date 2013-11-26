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

	float mass = 10;
	float radius = 1;
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

			shared_ptr<Camera> camera = Game::Instance()->camera;

			if(z > camera->position.z)
			{
				z = camera->position.z - 50;
			}

			palm->position = glm::vec3(x,y,z) ;
		}
	}
	
	GameComponent::Update(timeDelta);
}