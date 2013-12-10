#pragma once
#include "FPSLeapController.h"
#include "Game.h"
#include "Utils.h"
#include "Content.h"
#include "FountainEffect.h"

using namespace BGE;

FPSLeapController::FPSLeapController(Leap::Controller leapmotionController, PhysicsFactory * physicsFactory)
{
	this->leapmotionController = leapmotionController;
	this->physicsFactory = physicsFactory;
	this->pickedUp = NULL;
	this->gunSelection = 1;
	this->triggerPulled = false;
	this->elapsed = 10000.0f;
	this->jointGunSphere = NULL;
}

bool FPSLeapController::Initialise()
{
	UpdateFromParent();
	return GameComponent::Initialise();
}

FPSLeapController::~FPSLeapController(void)
{
}

void FPSLeapController::Update(float timeDelta)
{

	//LineDrawer::DrawLine(, glm::vec3(255,255,0));

	const Uint8 * keyState = Game::Instance()->GetKeyState();

	if (keyState[SDL_SCANCODE_1])
	{
		gunSelection = 1;
	}
	if (keyState[SDL_SCANCODE_2])
	{
		gunSelection = 2;
	}
	if (keyState[SDL_SCANCODE_3])
	{
		gunSelection = 3; 
	}
	if (keyState[SDL_SCANCODE_4])
	{
		gunSelection = 4; 
	}
	if (keyState[SDL_SCANCODE_5])
	{
		gunSelection = 5; 
	}
	if (keyState[SDL_SCANCODE_6])
	{
		gunSelection = 6; 
	}

	if(leapmotionController.isConnected())
	{
		int midX = Game::Instance()->GetWidth() / 2;
		int midY = Game::Instance()->GetHeight() / 2;

		Leap::Frame frameNew = leapmotionController.frame();
		Leap::Frame frameOld = leapmotionController.frame(1);

		//Camera controlls
		//Fix This
		Leap::Vector palmPosNew = frameNew.hands().rightmost().palmPosition();
		Leap::Vector palmPosOld = frameOld.hands().rightmost().palmPosition();

		if(palmPosOld == Leap::Vector(0,0,0))
		{
			palmPosOld = palmPosNew;
		}
		
		int x = palmPosOld.x - palmPosNew.x;
		int y = palmPosNew.y - palmPosOld.y; 

		float scale = 0.9f;
		if (x != 0)
		{
			parent->GetController()->Yaw(x * scale);
		}
		if (y != 0)
		{
			parent->GetController()->Pitch(y * scale);
		}

		// camera controlls end

		//Gun
		//This gun activates when 2 fingers change to One
		Leap::Hand rightHandNew = frameNew.hands().rightmost();

		Leap::Finger thumbNew = rightHandNew.fingers().leftmost();
		Leap::Finger indexNew = rightHandNew.fingers().frontmost();

		switch(gunSelection)
		{
		case 1:
			{
				Game::Instance()->PrintText("Gravity Gun");
				GravityGun( thumbNew, indexNew);
				break;
			}
		case 2:
			{
				Game::Instance()->PrintText("Joint Gun");
				JointGun( thumbNew, indexNew);
				break;
			}
		case 3:
			{
				Game::Instance()->PrintText("Sphere Gun");
				SphereGun( thumbNew, indexNew, timeDelta);
				break;
			}
		case 4:
			{
				Game::Instance()->PrintText("RagDoll Gun");
				RagDollGun( thumbNew, indexNew, timeDelta);
				break;
			}
		case 5:
			{
				Game::Instance()->PrintText("Morph Gun");
				MorphGun( thumbNew, indexNew, timeDelta);
				break;
			}
		case 6:
			{
				Game::Instance()->PrintText("Patri Gun");
				PartiGun( thumbNew, indexNew, timeDelta);
				break;
			}
		}
	}
	GameComponent::Update(timeDelta);
}

void FPSLeapController::GravityGun(Leap::Finger thumbNew, Leap::Finger indexNew)
{
	if(thumbNew.isValid() && indexNew.isValid())
		{
			if(thumbNew.id() == indexNew.id())
			{
				float dist = 1000.0f;
				if (pickedUp == NULL)
				{
					btVector3 rayFrom = GLToBtVector(parent->position + (parent->look * 4.0f));
					btVector3 rayTo = GLToBtVector(parent->position + (parent->look * dist));

					btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);
					physicsFactory->dynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);

					if (rayCallback.hasHit())
					{
						pickedUp = reinterpret_cast<PhysicsController*>(rayCallback.m_collisionObject->getUserPointer());
						if (pickedUp->parent == Game::Instance()->GetGround().get())
						{
							pickedUp = NULL;
						}
					}
				}
				if (pickedUp != NULL)
				{
					float powerfactor = 4.0f; // Higher values causes the targets moving faster to the holding point.
					float maxVel = 3.0f;      // Lower values prevent objects flying through walls.
					float holdDist = 80.0f;

					// Calculate the hold point in front of the camera
					glm::vec3 holdPos = parent->position + (parent->look * holdDist);

					glm::vec3 v = holdPos - pickedUp->position; // direction to move the Target
					v *= powerfactor; // powerfactor of the GravityGun

					if (v.length() > maxVel)
					{
						// if the correction-velocity is bigger than maximum
						v = glm::normalize(v);
						v *= maxVel; // just set correction-velocity to the maximum
					}
					pickedUp->rigidBody->setLinearVelocity(GLToBtVector(v));    
					pickedUp->rigidBody->activate();		
				}
			}
			else if(!pickedUp == NULL)
			{    
				pickedUp->rigidBody->applyCentralForce(GLToBtVector(parent->look) * 10000.0f);
				pickedUp = NULL;
			}
			else
			{
				pickedUp = NULL;
			}

		}
}

void FPSLeapController::JointGun(Leap::Finger thumbNew, Leap::Finger indexNew)
{
	if(thumbNew.isValid() && indexNew.isValid())
		{
			if(thumbNew.id() == indexNew.id())
			{
				float dist = 1000.0f;

				if(jointGunSphere == NULL)
				{
					glm::vec3 pos = parent->position + (parent->look * 60.0f);
					jointGunSphere = physicsFactory->CreateKinematicSphere(3, pos);
				}
				else
				{
					btVector3 rayFrom = GLToBtVector(parent->position + (parent->look * 4.0f));
					btVector3 rayTo = GLToBtVector(parent->position + (parent->look * dist));

					btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);
					physicsFactory->dynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);

					PhysicsController * jointGunObj = NULL;

					if (rayCallback.hasHit())
					{
						jointGunObj = reinterpret_cast<PhysicsController*>(rayCallback.m_collisionObject->getUserPointer());
						if (jointGunObj->parent == Game::Instance()->GetGround().get())
						{
							jointGunObj = NULL;
						}
					}

					if(jointGunObj != NULL)
					{
						if(!jointGunObj->rigidBody->isKinematicObject())
						{
							shared_ptr<PhysicsController> chainObject = physicsFactory->CreateCapsule(3,10, glm::vec3(0,0,0), glm::quat());

							btPoint2PointConstraint * p2p;

							p2p = new btPoint2PointConstraint(*jointGunSphere->rigidBody,*chainObject->rigidBody, btVector3(0,-3,0),btVector3(0,10,0));
							physicsFactory->dynamicsWorld->addConstraint(p2p);

							p2p = new btPoint2PointConstraint(*chainObject->rigidBody,*jointGunObj->rigidBody, btVector3(0,-10,0),btVector3(0,3,0));
							physicsFactory->dynamicsWorld->addConstraint(p2p);
	
							jointGunObj = NULL;
							jointGunSphere = NULL;
						}
					}
				}
			}	
		}
}

void FPSLeapController::SphereGun(Leap::Finger thumbNew, Leap::Finger indexNew,  float timeDelta)
{
	if(thumbNew.isValid() && indexNew.isValid())
	{
		float timeToPass = 1.0f / 5.0f;

		if(thumbNew.id() == indexNew.id() && elapsed > timeToPass)
		{
			glm::vec3 pos = parent->position + (parent->look * 5.0f);
			glm::quat q(RandomFloat(), RandomFloat(), RandomFloat(), RandomFloat());
			glm::normalize(q);
			shared_ptr<PhysicsController> physicsComponent = physicsFactory->CreateSphere(1, pos , q);
		
			float force = 5000.0f;
			physicsComponent->rigidBody->applyCentralForce(GLToBtVector(parent->look) * force);
			elapsed = 0.0f;
		}
		else
		{
			elapsed += timeDelta;
		}
	}
}

void FPSLeapController::RagDollGun(Leap::Finger thumbNew, Leap::Finger indexNew,  float timeDelta) 
{
	if(thumbNew.isValid() && indexNew.isValid())
	{
		float timeToPass = 1.0f / 2.0f;

		if(thumbNew.id() == indexNew.id() && elapsed > timeToPass)
		{
			glm::vec3 pos = parent->position + (parent->look * 5.0f);
			shared_ptr<PhysicsController> physicsComponent = physicsFactory->CreateRagDoll(pos);
		
			float force = 8000.0f;
			physicsComponent->rigidBody->applyCentralForce(GLToBtVector(parent->look) * force);
			elapsed = 0.0f;
		}
		else
		{
			elapsed += timeDelta;
		}
	}
}

void FPSLeapController::MorphGun(Leap::Finger thumbNew, Leap::Finger indexNew,  float timeDelta)
{
	if(thumbNew.isValid() && indexNew.isValid())
		{
			if(thumbNew.id() == indexNew.id() && triggerPulled == false)
			{

				float dist = 500.0f;
				PhysicsController * morphObject = NULL;

				btVector3 rayFrom = GLToBtVector(parent->position + (parent->look * 4.0f));
				btVector3 rayTo = GLToBtVector(parent->position + (parent->look * dist));

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

				triggerPulled = true;
			}
			else
			{
				triggerPulled = false;
			}
		}
}

void FPSLeapController::PartiGun(Leap::Finger thumbNew, Leap::Finger indexNew,  float timeDelta)
{
	if(thumbNew.isValid() && indexNew.isValid())
	{
		if(thumbNew.id() == indexNew.id())
		{
			if(triggerPulled == false)
			{
				float dist = 1000.0f;
				PhysicsController * partiObject = NULL;

				btVector3 rayFrom = GLToBtVector(parent->position + (parent->look * 4.0f));
				btVector3 rayTo = GLToBtVector(parent->position + (parent->look * dist));

				btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);
				physicsFactory->dynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);

				if (rayCallback.hasHit())
				{
					partiObject = reinterpret_cast<PhysicsController*>(rayCallback.m_collisionObject->getUserPointer());
					if (partiObject->parent == Game::Instance()->GetGround().get())
					{
						partiObject = NULL;
					}
				}

				if(partiObject != NULL)
				{						
						shared_ptr<FountainEffect> parti = make_shared<FountainEffect>(100);
						parti->position = partiObject->position + glm::vec3(0,5,0);
						parti->diffuse = glm::vec3(255,255, 0);
						partiObject->parent->Attach(parti);
				}

				triggerPulled = true;
			}
		}
		else
		{
			triggerPulled = false;
		}
	}
}