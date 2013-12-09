#pragma once
#include "FPSLeapController.h"
#include "Game.h"
#include "Utils.h"

using namespace BGE;

FPSLeapController::FPSLeapController(Leap::Controller leapmotionController, PhysicsFactory * physicsFactory)
{
	this->leapmotionController = leapmotionController;
	this->physicsFactory = physicsFactory;
	this->pickedUp = NULL;
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

		Game::Instance()->PrintFloat("Leap x ", x);
		Game::Instance()->PrintFloat("Leap y ", y);

		float scale = 0.4f;
		if (x != 0)
		{
			parent->GetController()->Yaw(x * scale);
		}
		if (y != 0)
		{
			parent->GetController()->Pitch(y * scale);
		}

		// camera controlls end

		//GravityGun

		//This gun activates when 2 fingers change to One
		Leap::Hand rightHandNew = frameNew.hands().rightmost();
		Leap::Hand rightHandOld = frameOld.hands().rightmost();

		Leap::Finger thumbNew = rightHandNew.fingers().leftmost();
		Leap::Finger indexNew = rightHandNew.fingers().frontmost();

		if(thumbNew.isValid() && indexNew.isValid())
		{
			if(thumbNew.id() == indexNew.id())
			{
				Game::Instance()->PrintText("Shootign");

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
					float holdDist = 200.0f;

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
					//what = pickedUp->tag;	
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
		//GravityGun end
	}
	GameComponent::Update(timeDelta);
}
