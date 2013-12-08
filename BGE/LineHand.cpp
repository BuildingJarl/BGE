#include "LineHand.h"
#include "Camera.h"
#include "Utils.h"

using namespace BGE;

LineHand::LineHand(Leap::Controller leapmotionController ,btDiscreteDynamicsWorld * dynamicsWorld)
{
	this->leapmotionController = leapmotionController;
	this->dynamicsWorld = dynamicsWorld;
	elapsedLeapFrameTime = 0;
	physicsFactory = make_shared<PhysicsFactory>(dynamicsWorld);
}

bool LineHand::Initialise()	
{
	palm = physicsFactory->CreateKinematicCylinder(10,1,glm::vec3(0,20,0));

	//Create finger tips
	thumbTip = physicsFactory->CreateKinematicSphere(1, glm::vec3(-5, 20, -12));
	indexTip = physicsFactory->CreateKinematicSphere(1, glm::vec3(-2, 20, -17));
	middleTip = physicsFactory->CreateKinematicSphere(1, glm::vec3(0, 20, -17));
	ringTip = physicsFactory->CreateKinematicSphere(1, glm::vec3(2, 20, -17));
	pinkyTip = physicsFactory->CreateKinematicSphere(1, glm::vec3(4, 20, -16));

	return true;
}

LineHand::~LineHand(void)
{
}

void LineHand::Update(float timeDelta) 
{

	if(leapmotionController.isConnected())
	{
		Leap::Frame fNew = leapmotionController.frame();

		Leap::Vector palmPosNew = fNew.hands()[0].palmPosition();

		palm->parent->position = glm::vec3(palmPosNew.x, palmPosNew.y - 200, palmPosNew.z);
		

		Leap::FingerList fingerListNow = fNew.hands()[0].fingers();

		std::vector<glm::vec3> fingerPositions;

		for(int f = 0; f < fingerListNow.count(); f++)
		{
			Leap::Vector fp = fingerListNow[f].tipPosition();
			fingerPositions.push_back(glm::vec3(fp.x ,fp.y - 200,fp.z ));
		}

		std::sort(fingerPositions.begin(),fingerPositions.end(),XSmallerLH());
		int index = 0;

		if(fingerPositions.size() > 0)
		{
			thumbTip->parent->position = fingerPositions[0];
			index++;
			LineDrawer::DrawLine(palm->parent->position,thumbTip->parent->position, glm::vec3(255,255,0));
		}
		if(fingerPositions.size() > 1)
		{
			indexTip->parent->position = fingerPositions[1];
			index++;
			LineDrawer::DrawLine(palm->parent->position,indexTip->parent->position, glm::vec3(255,255,0));
		}
		if(fingerPositions.size() > 2)
		{
			middleTip->parent->position = fingerPositions[2];
			index++;
			LineDrawer::DrawLine(palm->parent->position,middleTip->parent->position, glm::vec3(255,255,0));
		}
		if(fingerPositions.size() > 3)
		{
			ringTip->parent->position = fingerPositions[3];
			index++;
			LineDrawer::DrawLine(palm->parent->position,ringTip->parent->position, glm::vec3(255,255,0));
		}
		if(fingerPositions.size() > 4)
		{
			pinkyTip->parent->position = fingerPositions[4];
			index++;
			LineDrawer::DrawLine(palm->parent->position,pinkyTip->parent->position, glm::vec3(255,255,0));
		}

	}
	GameComponent::Update(timeDelta);
}
