#include "Hand.h"
#include "Camera.h"
#include "Utils.h"

using namespace BGE;

Hand::Hand(Leap::Controller leapmotionController ,btDiscreteDynamicsWorld * dynamicsWorld)
{
	this->leapmotionController = leapmotionController;
	this->dynamicsWorld = dynamicsWorld;
	physicsFactory = make_shared<PhysicsFactory>(dynamicsWorld);
}

bool Hand::Initialise()
{
	//CreateHand(glm::vec3(0,10,0),glm::quat());
	shared_ptr<PhysicsController> tes = physicsFactory->CreateCylinder(5,3,glm::vec3(0,0,0), glm::quat());
	shared_ptr<GameComponent> palm = physicsFactory->CreateKinematicCylinder(5,3,glm::vec3(0,0,0), glm::quat());
	shared_ptr<GameComponent> shp = physicsFactory->CreateKinematicSphere(3,glm::vec3(0,0,0), glm::quat());

	return true;
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
		/*
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
			//palm->GetController()->position = palmPos;
			Leap::FingerList fingerList = hand.fingers();
			if(fingerList.count() > 1)
			{
				Leap::Finger finger = hand.fingers().leftmost();
				Leap::Vector pos = finger.tipPosition();
				
				fingerTip->position.x = pos.x;
				fingerTip->position.y = pos.y - 200;
				fingerTip->position.z = pos.z / 3;
			}
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
		}
	*/
	}

	GameComponent::Update(timeDelta);
}

/*
void Hand::CreateHand(glm::vec3 pos, glm::quat quat)
{
	//palm
	float palmRadius = 5;
	float palmHeight = 2;
	btVector3 inerta(0,0,0);
	glm::vec3 palmPostion = pos;
	shared_ptr<KinematicController> palmController = CreatePalm(palmRadius,palmHeight,palmPostion, quat);


	glm::vec3 fingerPostion = glm::vec3(0,0,0);
	float fingerHeight = 10;
	float fingerRadius = 1;
	float heightLower = (fingerHeight / 2) - 1;
	float heightUpper = (fingerHeight / 2) + 1;
	glm::vec3 posLower = glm::vec3(0,10,-10); //toDo
	glm::vec3 posUpper = glm::vec3(0,10,-20); //toDo

	//glm::quat q =  glm::angleAxis(glm::half_pi<float>(), glm::vec3(1, 0, 0));

	//thumb
	shared_ptr<PhysicsController> thumbLower = CreateCapsule(heightLower, fingerRadius, posLower, quat);
	shared_ptr<PhysicsController> thumbUpper = CreateCapsule(heightUpper, fingerRadius, posUpper, quat);
	//thumb joint
	//btHingeConstraint * hinge = new btHingeConstraint(*thumbLower->rigidBody, *thumbUpper->rigidBody, btVector3(0,2,0), btVector3(0,-2,0), btVector3(0,1,0), btVector3(0,1,0), true);
    //dynamicsWorld->addConstraint(hinge);
	btPoint2PointConstraint * ptpConstraint = new btPoint2PointConstraint(*thumbLower->rigidBody, *thumbUpper->rigidBody, btVector3(0,3,0),btVector3(0,-3,0));
    dynamicsWorld->addConstraint(ptpConstraint);
	//palm thumb joints
	ptpConstraint = new btPoint2PointConstraint(*palmController->rigidBody, *thumbLower->rigidBody, btVector3(-3,0,-5),btVector3(0,-2,0));
    dynamicsWorld->addConstraint(ptpConstraint);

	shared_ptr<KinematicController> tipController = CreateFingerTip(1,glm::vec3(0,10,-20), quat);

	ptpConstraint = new btPoint2PointConstraint( *thumbUpper->rigidBody ,*tipController->rigidBody, btVector3(0,2,0),btVector3(0,2,0));
    dynamicsWorld->addConstraint(ptpConstraint);

}

shared_ptr<KinematicController> Hand::CreateFingerTip(float tipRadius, glm::vec3 pos, glm::quat quat)
{
	float mass = 10;
	float radius = 1;
	btVector3 inertia(0,0,0);

	//create container for palm (what wee see on screen)
	fingerTip = shared_ptr<GameComponent> (new Sphere(radius));;
	fingerTip->Attach(Content::LoadModel("sphere", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0,1,0))));
	fingerTip->position = pos;
	Attach(fingerTip); // attach to game

	//create collisionShape for sphere
	btCollisionShape * palmColShape = new btSphereShape(radius);
	palmColShape->calculateLocalInertia(mass,inertia);

	//Create PhysicsHand component and attach to sphere
	shared_ptr<KinematicController> physicsHand = make_shared<KinematicController>();
	fingerTip->Attach(physicsHand);

	//create a rigid body
	btRigidBody::btRigidBodyConstructionInfo palmCI(mass,physicsHand.get(),palmColShape,inertia);
	btRigidBody * body = new btRigidBody(palmCI);
	physicsHand->SetPhysicsStuff(palmColShape,body,physicsHand.get());
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	body->setActivationState(DISABLE_DEACTIVATION);
	
	//attach to world
	this->dynamicsWorld->addRigidBody(body);

	return physicsHand;
}

shared_ptr<KinematicController> Hand::CreatePalm(float palmRadius,float palmHeight, glm::vec3 pos, glm::quat quat)
{
	float mass = 1;
	btVector3 inerta(0,10,0);
	
	//Create Shape
	btCollisionShape * shape = new btCylinderShape(btVector3(palmRadius, palmHeight*0.5f, palmRadius));
	shape->calculateLocalInertia(mass,btVector3(0,0,0));

	//Create Container
	palm = make_shared<GameComponent>(Cylinder(palmRadius,palmHeight)); 
	palm->position = pos;
	Attach(palm);

	//Create PhysicsHand component and attach to container
	shared_ptr<PhysicsHand> physicsHand = make_shared<PhysicsHand>();
	physicsHand->tag = "Palm";
	palm->Attach(physicsHand);

	//Create Rigid Body
	//btDefaultMotionState * motionState = new btDefaultMotionState(btTransform(GLToBtQuat(quat), GLToBtVector(pos)));
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass,  physicsHand.get(), shape, btVector3(0,0,0));
	btRigidBody * body = new btRigidBody(rigidBodyCI);
	physicsHand->SetPhysicsStuff(shape,body,physicsHand.get());
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	body->setActivationState(DISABLE_DEACTIVATION);

	//Add to Bullet World
	dynamicsWorld->addRigidBody(body);
	return physicsHand;
}

shared_ptr<PhysicsController> Hand::CreateFinger(float fingerHeight,float fingerRadius, glm::vec3 fingerPostion, glm::quat quat)
{
	float heightLower = (fingerHeight / 2) - 1;
	float heightUpper = (fingerHeight / 2) + 1;
	
	glm::vec3 posLower = glm::vec3(0,10,-10); //toDo
	glm::vec3 posUpper = glm::vec3(0,10,-20); //toDo

	shared_ptr<PhysicsController> fingerLower = CreateCapsule(heightLower, fingerRadius, posLower, quat);
	shared_ptr<PhysicsController> fingerUpper = CreateCapsule(heightUpper, fingerRadius, posUpper, quat);
	
	
	shared_ptr<PhysicsController> finger = make_shared<PhysicsController>();

	finger->Attach(fingerLower);
	finger->Attach(fingerUpper); 

	 // TORSO
     // Connecting the torso to the (upper) left leg via a ball and socket hinge.
    // btPoint2PointConstraint * ptpConstraint = new btPoint2PointConstraint(*fingerLower->rigidBody, *upperLeftLeg->rigidBody, btVector3(-1,-2.75f,0),btVector3(0,3,0));
     //dynamicsWorld->addConstraint(ptpConstraint);

	return finger;
}

shared_ptr<PhysicsController> Hand::CreateCapsule(float height,float radius,glm::vec3 pos,glm::quat quat)
{

	shared_ptr<PhysicsController> capsule = physicsFactory->CreateCylinder(radius, height, pos, quat);
	return capsule;

	//make bullet shape
	btCollisionShape * capsuleShape = new btCapsuleShape(btScalar(radius),btScalar(height));
	btScalar mass = 1;
	btVector3 inerta(0,0,0);
	capsuleShape->calculateLocalInertia(mass,inerta);

	//combine cylinder and sphere to make the model (container for bullet)
	//body
	shared_ptr<Cylinder> capsuleBody = make_shared<Cylinder>(Cylinder(radius,height));
	capsuleBody->position = pos;
	Game::Instance()->Attach(capsuleBody);

	//cap one
	shared_ptr<Sphere> sphere1 = make_shared<Sphere>(radius);
	//sphere1->worldMode = GameComponent::from_self_with_parent;
	sphere1->position = glm::vec3(0, +(height-radius)-0.25f,0);
	capsuleBody->Attach(sphere1);
	
	//cap two
	shared_ptr<Sphere> sphere2 = make_shared<Sphere>(radius);
	//sphere2->worldMode = GameComponent::from_self_with_parent;
	sphere2->position = glm::vec3(0, -(height-radius)+0.25f,0);
	capsuleBody->Attach(sphere2);

	//create Bullet Body
	btDefaultMotionState * motionState = new btDefaultMotionState(btTransform(GLToBtQuat(quat), GLToBtVector(pos)));
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass,  motionState, capsuleShape, btVector3(0,0,0));
	btRigidBody * body = new btRigidBody(rigidBodyCI);
	dynamicsWorld->addRigidBody(body);

	//Create PhysicsController
	shared_ptr<PhysicsController> fingerController = make_shared<PhysicsController>(PhysicsController(capsuleShape, body, motionState));
	fingerController->tag = "finger";
	body->setUserPointer(fingerController.get());
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	capsuleBody->Attach(fingerController);

	return fingerController;
}
*/