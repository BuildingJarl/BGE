#include "Hand.h"
#include "Camera.h"
#include "Utils.h"


using namespace BGE;

Hand::Hand(Leap::Controller leapmotionController , PhysicsFactory * physicsFactory)
{
	this->leapmotionController = leapmotionController;
	this->physicsFactory = physicsFactory;

	this->gunSelection = 1;
	this->elapsedTime = 0;
	this->elapsedTimeSelection = 0;
	this->elapsedJointGun= 0;
	this->elapsedSphereGun = 0;
	this->elapsedRagDollLauncher = 0;
	this->elapsedMorphGun = 0;
	this->pickedUpGravityGun = NULL;
}

bool Hand::Initialise()	
{

	palm = make_shared<ivanPatri>(500);
	palm->position = glm::vec3(0, 10, 0);
	palm->diffuse = glm::vec3(0,1, 0);
	parent->Attach(palm);

	for(int i = 0; i < 5; i++)
	{
		shared_ptr<ivanPatri> eff = make_shared<ivanPatri>(500);
		eff->position = glm::vec3(0, 10, 0);

		if(i % 2 == 1)
		{
			eff->diffuse = glm::vec3(0, 0, 1);
		}
		else
		{
			eff->diffuse = glm::vec3(1, 0, 0);
		}
		
		parent->Attach(eff);
		fingerList.push_back(eff);
	}

	pinky = physicsFactory->CreateKinematicSphere(1,glm::vec3(-8,0,-3));

	return true;
}

Hand::~Hand(void)
{
}

void Hand::Update(float timeDelta) 
{
	GunSelection(timeDelta);
	Game::Instance()->camera->GetController()->position = (palm->position + (palm->look * -100.0f)) + glm::vec3(0,30,0);

	if(leapmotionController.isConnected())
	{
		Leap::Frame f = leapmotionController.frame();

		Leap::Hand hand = f.hands().rightmost();

		Leap::Vector pos = hand.palmPosition();
		palm->position = glm::vec3(pos.x,pos.y-100,pos.z);

		

		for(int f = 0; f < hand.fingers().count(); f++)
		{
			Leap::Finger finger = hand.fingers()[f];
			Leap::Vector fp = finger.tipPosition();

			fingerList[f]->position = glm::vec3(fp.x,fp.y-100,fp.z);
			LineDrawer::DrawLine(palm->position, glm::vec3(fp.x,fp.y-100,fp.z), glm::vec3(153,0,0));

			if(finger == hand.fingers().leftmost())
			{
				thumb = fingerList[f];
			}
			else if(finger == hand.fingers().frontmost())
			{
				middle = fingerList[f];
			}
			else if(finger == hand.fingers().rightmost())
			{
				//middle = fingerList[f];
				pinky->parent->position = glm::vec3(fp.x,fp.y-100,fp.z);
			}

		}


		Leap::Finger thumbF = hand.fingers().leftmost();
		Leap::Finger middleF = hand.fingers().frontmost();

		if( thumbF.id() == middleF.id() && thumbF.isValid() && middleF.isValid())
		{
			Game::Instance()->PrintText("Shooting");
			float distance = 100.0f;
			if(middle != NULL)
			{
				LineDrawer::DrawLine(middle->position,middle->position + (middle->look * distance), glm::vec3(255,255,0));
			}
			
			switch(gunSelection)
			{
				case 1:
				{
					GravityGun();
					break;
				}
				case 2:
				{
					JointGun(timeDelta);
					break;
				}
				case 3:
				{
					SphereGun(timeDelta);
					break;
				}
				case 4:
				{
					RagDollLauncher(timeDelta);
					break;
				}
				case 5:
				{
					MorphGun(timeDelta);
					break;
				}
			}
		}
		else
		{
			pickedUpGravityGun = NULL;

		}
	}

	GameComponent::Update(timeDelta);
}

void Hand::GunSelection(float timeDelta)
{
	const Uint8 * keyState = Game::Instance()->GetKeyState();

	float timeToPass = 1.0f / 2.0f;

	if(elapsedTimeSelection > timeToPass)
	{
		if (keyState[SDL_SCANCODE_TAB])
		{
			gunSelection++;
			if(gunSelection > 5)
			{
				gunSelection = 1;
			}
		}
		elapsedTimeSelection = 0;
	}
	else
	{
		elapsedTimeSelection += timeDelta;
	}
	

	switch(gunSelection)
	{
		case 1:
			{
				Game::Instance()->PrintText("Gravity Gun");
				break;
			}
		case 2:
			{
				Game::Instance()->PrintText("Joint Gun");
				break;
			}
		case 3:
			{
				Game::Instance()->PrintText("Sphere Gun");
				break;
			}
		case 4:
			{
				Game::Instance()->PrintText("RagDoll Launcher");
				break;
			}
		case 5:
			{
				Game::Instance()->PrintText("Morph Gun");
				break;
			}
	}
}

void Hand::GravityGun()
{
	float distance = 1000.0f;

	if (pickedUpGravityGun == NULL)
	{
		btVector3 rayFrom = GLToBtVector(middle->position + (middle->look * 80.0f));
		btVector3 rayTo = GLToBtVector(middle->position + (middle->look * distance));

		btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);
		physicsFactory->dynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);

		if (rayCallback.hasHit())
		{
			pickedUpGravityGun = reinterpret_cast<PhysicsController*>(rayCallback.m_collisionObject->getUserPointer());
			if (pickedUpGravityGun->parent == Game::Instance()->GetGround().get())
			{
				pickedUpGravityGun = NULL;
			}
		}
	}
	if (pickedUpGravityGun != NULL)
	{
		float powerfactor = 4.0f; // Higher values causes the targets moving faster to the holding point.
		float maxVel = 3.0f;      // Lower values prevent objects flying through walls.
		float holdDist = 80.0f;

		// Calculate the hold point in front of the finger
		glm::vec3 holdPos = middle->position + (middle->look * holdDist);

		glm::vec3 v = holdPos - pickedUpGravityGun->position; // direction to move the Target
		v *= powerfactor; // powerfactor of the GravityGun

		if (v.length() > maxVel)
		{
			// if the correction-velocity is bigger than maximum
			v = glm::normalize(v);
			v *= maxVel; // just set correction-velocity to the maximum
		}
		pickedUpGravityGun->rigidBody->setLinearVelocity(GLToBtVector(v));    
		pickedUpGravityGun->rigidBody->activate();		
	}
}

void Hand::JointGun(float timeDelta)
{
	float timeToPass = 1.0f;

	if(elapsedJointGun > timeToPass)
	{
		float dist = 1000.0f;
		btVector3 rayFrom = GLToBtVector(middle->position + (middle->look * 4.0f));
		btVector3 rayTo = GLToBtVector(middle->position + (middle->look * dist));

		btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);
		physicsFactory->dynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);
		PhysicsController * selectedJointGunOne = NULL;
		if (rayCallback.hasHit())
		{
			selectedJointGunOne = reinterpret_cast<PhysicsController*>(rayCallback.m_collisionObject->getUserPointer());
			if (selectedJointGunOne->parent == Game::Instance()->GetGround().get())
			{
				selectedJointGunOne = NULL;
			}
			if(selectedJointGunOne->rigidBody->isKinematicObject())
			{
				selectedJointGunOne = NULL;
			}
		}

		if(selectedJointGunOne != NULL)
		{
			glm::vec3 pos = middle->position + (middle->look * 5.0f);
			shared_ptr<PhysicsController> physicsComponent = physicsFactory->CreateSphere(3, pos , glm::quat());
		
			shared_ptr<PhysicsController> chainObject = physicsFactory->CreateCapsule(1,10, pos, glm::quat());

			btPoint2PointConstraint * p2p;

			p2p = new btPoint2PointConstraint(*selectedJointGunOne->rigidBody,*chainObject->rigidBody, btVector3(0,-3,0),btVector3(0,10,0));
			physicsFactory->dynamicsWorld->addConstraint(p2p);

			p2p = new btPoint2PointConstraint(*chainObject->rigidBody,*physicsComponent->rigidBody, btVector3(0,-10,0),btVector3(0,3,0));
			physicsFactory->dynamicsWorld->addConstraint(p2p);

			float force = 5000.0f;
			physicsComponent->rigidBody->applyCentralForce(GLToBtVector(middle->parent->look) * force);
			
			elapsedJointGun = 0;
		}
	}
	else
	{
		elapsedJointGun += timeDelta;
	}
}

void Hand::SphereGun(float timeDelta)
{
	float timeToPass = 1.0f / 6.0f;

	if(elapsedSphereGun > timeToPass)
	{
		glm::vec3 pos = middle->position + (middle->look * 5.0f);
		shared_ptr<PhysicsController> physicsComponent = physicsFactory->CreateSphere(0.5f, pos , glm::quat());
		
		float force = 8000.0f;
		physicsComponent->rigidBody->applyCentralForce(GLToBtVector(middle->look) * force);
		elapsedSphereGun = 0.0f;
	}
	else
	{
		elapsedSphereGun += timeDelta;
	}
}

void Hand::RagDollLauncher(float timeDelta)
{
	float timeToPass = 1.0f / 2.0f;

	if(elapsedRagDollLauncher > timeToPass)
	{
		glm::vec3 pos = middle->position + (middle->look * 5.0f);
		shared_ptr<PhysicsController> physicsComponent = physicsFactory->CreateRagDoll(pos);
		
		float force = 10000.0f;
		physicsComponent->rigidBody->applyCentralForce(GLToBtVector(middle->look) * force);
		elapsedRagDollLauncher = 0.0f;
	}
	else
	{
		elapsedRagDollLauncher += timeDelta;
	}
}

void Hand::MorphGun(float timeDelta)
{
	float timeToPass = 1.0f / 2.0f;

	if(elapsedMorphGun > timeToPass)
	{
		float dist = 500.0f;

		PhysicsController * morphObject = NULL;

		btVector3 rayFrom = GLToBtVector(middle->position + (middle->look * 4.0f));
		btVector3 rayTo = GLToBtVector(middle->position + (middle->look * dist));

		btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);
		physicsFactory->dynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);

		if (rayCallback.hasHit())
		{
			morphObject = reinterpret_cast<PhysicsController*>(rayCallback.m_collisionObject->getUserPointer());
			if (morphObject->parent == Game::Instance()->GetGround().get())
			{
				morphObject = NULL;
			}
		}

		if(morphObject != NULL)
		{
			glm::vec3 pos = morphObject->position;

			//delete previous object
			physicsFactory->dynamicsWorld->removeRigidBody(morphObject->rigidBody);
			morphObject->parent->alive = false;

			physicsFactory->CreateCylinder(3,4,pos,glm::quat());
		}
	}
	else
	{
		elapsedMorphGun += timeDelta;
	}
}