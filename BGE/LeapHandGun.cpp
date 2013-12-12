#include "LeapHandGun.h"
using namespace BGE;

LeapHandGun::LeapHandGun(Leap::Controller leapmotionController , PhysicsFactory * physicsFactory)
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

LeapHandGun::~LeapHandGun(void)
{
}

bool LeapHandGun::Initialise()
{

	
	palm = physicsFactory->CreateKinematicCylinder(3,1,glm::vec3(0,0,0));

	thumb = physicsFactory->CreateKinematicSphere(1,glm::vec3(-8,0,-3));
	middle = physicsFactory->CreateKinematicSphere(1,glm::vec3(0,0,-9));
	pinky = physicsFactory->CreateKinematicSphere(1,glm::vec3(3,0,-5));

	return true;
}

void LeapHandGun::Update(float timeDelta)
{
	GunSelection(timeDelta);

	float distance = 100.0f;

	LineDrawer::DrawLine(middle->parent->position,middle->parent->position + (middle->parent->look * distance), glm::vec3(255,255,0));

	Game::Instance()->camera->GetController()->position = (palm->parent->position + (palm->parent->look * -80.0f)) + glm::vec3(0,10,0);

	CalcPositions();

	if(leapmotionController.isConnected())
	{
		Leap::Frame frame = leapmotionController.frame();
		Leap::Hand rightHand = frame.hands().rightmost();
		Leap::Finger thumbF = rightHand.fingers().leftmost();
		Leap::Finger middleF = rightHand.fingers().frontmost();

		if( thumbF.id() == middleF.id())
		{
			Game::Instance()->PrintText("Shooting");

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

}

void LeapHandGun::CalcPositions()
{
	if(leapmotionController.isConnected())
	{
		Leap::Frame fNew = leapmotionController.frame();
		Leap::Frame fOld = leapmotionController.frame(1);

		Leap::Hand handNew = leapmotionController.frame().hands().rightmost();
		Leap::Hand handOld = leapmotionController.frame(1).hands().rightmost();

		Leap::Vector pos = handNew.translation(fOld);
		glm::vec3 bgePos = glm::vec3(pos.x,pos.y,pos.z);

		palm->parent->position += bgePos;
		thumb->parent->position += bgePos;
		middle->parent->position += bgePos;
		pinky->parent->position += bgePos;
	}
}

void LeapHandGun::GunSelection(float timeDelta)
{
	const Uint8 * keyState = Game::Instance()->GetKeyState();

	float timeToPass = 1.0f / 3.0f;

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

void LeapHandGun::GravityGun()
{
	float distance = 1000.0f;

	if (pickedUpGravityGun == NULL)
	{
		btVector3 rayFrom = GLToBtVector(middle->parent->position + (middle->parent->look * 80.0f));
		btVector3 rayTo = GLToBtVector(middle->parent->position + (middle->parent->look * distance));

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
		glm::vec3 holdPos = middle->parent->position + (middle->parent->look * holdDist);

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

void LeapHandGun::JointGun(float timeDelta)
{
	float timeToPass = 1.0f;

	if(elapsedJointGun > timeToPass)
	{
		float dist = 1000.0f;
		btVector3 rayFrom = GLToBtVector(middle->parent->position + (middle->parent->look * 4.0f));
		btVector3 rayTo = GLToBtVector(middle->parent->position + (middle->parent->look * dist));

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
			glm::vec3 pos = middle->parent->position + (middle->parent->look * 5.0f);
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

void LeapHandGun::SphereGun(float timeDelta)
{
	float timeToPass = 1.0f / 6.0f;

	if(elapsedSphereGun > timeToPass)
	{
		glm::vec3 pos = middle->parent->position + (middle->parent->look * 5.0f);
		shared_ptr<PhysicsController> physicsComponent = physicsFactory->CreateSphere(0.5f, pos , glm::quat());
		
		float force = 8000.0f;
		physicsComponent->rigidBody->applyCentralForce(GLToBtVector(middle->parent->look) * force);
		elapsedSphereGun = 0.0f;
	}
	else
	{
		elapsedSphereGun += timeDelta;
	}
}

void LeapHandGun::RagDollLauncher(float timeDelta)
{
	float timeToPass = 1.0f / 2.0f;

	if(elapsedRagDollLauncher > timeToPass)
	{
		glm::vec3 pos = middle->parent->position + (middle->parent->look * 5.0f);
		shared_ptr<PhysicsController> physicsComponent = physicsFactory->CreateRagDoll(pos);
		
		float force = 10000.0f;
		physicsComponent->rigidBody->applyCentralForce(GLToBtVector(middle->parent->look) * force);
		elapsedRagDollLauncher = 0.0f;
	}
	else
	{
		elapsedRagDollLauncher += timeDelta;
	}
}

void LeapHandGun::MorphGun(float timeDelta)
{
	float timeToPass = 1.0f / 2.0f;

	if(elapsedMorphGun > timeToPass)
	{
		float dist = 500.0f;

		PhysicsController * morphObject = NULL;

		btVector3 rayFrom = GLToBtVector(middle->parent->position + (middle->parent->look * 4.0f));
		btVector3 rayTo = GLToBtVector(middle->parent->position + (middle->parent->look * dist));

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
