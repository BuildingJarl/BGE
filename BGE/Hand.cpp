#include "Hand.h"
#include "Camera.h"
#include "PhysicsHand.h"
#include "Content.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Utils.h"

using namespace BGE;

Hand::Hand(Leap::Controller leapmotionController, btDiscreteDynamicsWorld * dynamicsWorld)
{
	
	this->leapmotionController = leapmotionController;
	this->dynamicsWorld = dynamicsWorld;

	CreateHand(glm::vec3(0,20,0),glm::quat());

}


Hand::~Hand(void)
{
}

void Hand::Update(float timeDelta) 
{
	/*
	for(int i = 0; i< 5; i++)
	{
		fingers[i]->position;
		LineDrawer::DrawLine(palm->position,fingers[i]->position, glm::vec3(255,255,0));
	}
	*/

	if(leapmotionController.isConnected())
	{
		Leap::Frame f = leapmotionController.frame();
		Leap::HandList handList  = f.hands();

		if(handList.count() > 0)
		{
			Leap::Hand hand = handList[0];

			float x = hand.palmPosition().x;
			float y = (hand.palmPosition().y) - 200;
			float z = hand.palmPosition().z;
			glm::vec3 palmPos = glm::vec3(x,y,z);

			palm->position = palmPos;
			/*
			Leap::FingerList fingerList = hand.fingers();

			for(int f = 0; f < fingerList.count(); f++) {
				Leap::Finger finger = fingerList[f];
				Leap::Vector pos = finger.tipPosition(); //i think the position is in mm?

				fingers[f]->position.x = pos.x;
				fingers[f]->position.y = pos.y - 200;
				fingers[f]->position.z = pos.z;

				//LineDrawer::DrawLine(palm->position,fingers[f]->position, glm::vec3(255,255,0));
			};
			*/
		}
	}

	
	GameComponent::Update(timeDelta);
}

void Hand::CreateHand(glm::vec3 pos, glm::quat quat)
{
	float palmRadius = 5;
	float palmHeight = 2;
	float mass = 1;
	btVector3 inerta(0,0,0);

	glm::vec3 palmPostion = glm::vec3(0,0,0);
	glm::vec3 fingerPostion = glm::vec3(0,0,0);

	palm = CreatePalm(palmRadius,palmHeight,palmPostion, quat);
	

	float fingerHeight = 6;
	float fingerWidth = 1;

	shared_ptr<GameComponent> thumb = CreateFinger(fingerHeight, fingerWidth, fingerPostion, quat);
	//thumb->worldMode = GameComponent::from_self_with_parent;
	palm->Attach(thumb);
	
	Attach(palm);
}





shared_ptr<GameComponent> Hand::CreatePalm(float palmRadius,float palmHeight, glm::vec3 pos, glm::quat quat)
{
	float mass = 1;
	btVector3 inerta(0,0,0);
	
	//Create Shape
	btCollisionShape * shape = new btCylinderShape(btVector3(palmRadius, palmHeight*0.5f, palmRadius));
	shape->calculateLocalInertia(mass,btVector3(0,0,0));

	//Create Container
	shared_ptr<GameComponent> comp = make_shared<GameComponent>(Cylinder(palmRadius,palmHeight)); 
	comp->position = pos;
	//Attach(palm);
	
	//Create PhysicsHand component and attach to container
	shared_ptr<PhysicsHand> physicsHand = make_shared<PhysicsHand>();
	physicsHand->tag = "Palm";
	comp->Attach(physicsHand);

	//Create Rigid Body
	//btDefaultMotionState * motionState = new btDefaultMotionState(btTransform(GLToBtQuat(quat), GLToBtVector(pos)));
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass,  physicsHand.get(), shape, btVector3(0,0,0));
	btRigidBody * body = new btRigidBody(rigidBodyCI);
	physicsHand->SetPhysicsStuff(shape,body,physicsHand.get());
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	body->setActivationState(DISABLE_DEACTIVATION);

	//Add to Bullet World
	dynamicsWorld->addRigidBody(body);

	return comp;
	/*
	float mass = 10;
	float radius = 2;
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
	*/
}

shared_ptr<GameComponent> Hand::CreateFinger(float fingerHeight,float fingerRadius, glm::vec3 fingerPostion, glm::quat quat)
{
	float heightLower = (fingerHeight / 2) - 1;
	float heightUpper = (fingerHeight / 2) + 1;
	
	glm::vec3 posLower = glm::vec3(0,0,-10); //toDo
	glm::vec3 posUpper = glm::vec3(0,0,-20); //toDo

	shared_ptr<GameComponent> fingerLower = CreateCapsule(heightLower, fingerRadius, posLower, quat);
	shared_ptr<GameComponent> fingerUpper = CreateCapsule(heightUpper, fingerRadius, posUpper, quat);
	shared_ptr<GameComponent> finger = make_shared<GameComponent>();

	finger->Attach(fingerLower);
	finger->Attach(fingerUpper);

	//finger->Roll(90);
	return finger;
}

shared_ptr<GameComponent> Hand::CreateCapsule(float height,float radius,glm::vec3 pos,glm::quat quat)
{
	//make bullet shape
	btCollisionShape * capsuleShape = new btCapsuleShape(btScalar(radius),btScalar(height));
	btScalar mass = 1;
	btVector3 inerta(0,0,0);
	capsuleShape->calculateLocalInertia(mass,inerta);

	//combine cylinder and sphere to make the model (container for bullet)
	//body
	shared_ptr<Cylinder> capsuleContainer = make_shared<Cylinder>(Cylinder(radius,height)); 
	capsuleContainer->position = pos;
	
	//cap one
	shared_ptr<Sphere> sphere1 = make_shared<Sphere>(radius);
	//sphere1->worldMode = GameComponent::from_self_with_parent;
	sphere1->position = glm::vec3(0, +(height-radius)-0.25f,0);
	capsuleContainer->Attach(sphere1);
	
	//cap two
	shared_ptr<Sphere> sphere2 = make_shared<Sphere>(radius);
	//sphere2->worldMode = GameComponent::from_self_with_parent;
	sphere2->position = glm::vec3(0, -(height-radius)+0.25f,0);
	capsuleContainer->Attach(sphere2);
	capsuleContainer->Pitch(90);
	return capsuleContainer;
}