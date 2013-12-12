#include "Hand.h"
#include "Camera.h"
#include "Utils.h"

using namespace BGE;

Hand::Hand(Leap::Controller leapmotionController ,btDiscreteDynamicsWorld * dynamicsWorld)
{
	this->leapmotionController = leapmotionController;
	this->dynamicsWorld = dynamicsWorld;
	elapsedLeapFrameTime = 0;
	physicsFactory = make_shared<PhysicsFactory>(dynamicsWorld);
}

bool Hand::Initialise()	
{
	btPoint2PointConstraint * ptpConstraint;
	palm = physicsFactory->CreateKinematicCylinder(6,1,glm::vec3(0,40,20));

	float fingerLowerHeight = 14;
	float fingerUpperHeight = 8;
	/*
	//create lower fingers
	thumbLower = physicsFactory->CreateCapsule(0.5f, fingerLowerHeight, glm::vec3(0, 0, 0), glm::quat());
	indexLower = physicsFactory->CreateCapsule(0.5f, fingerLowerHeight, glm::vec3(0, 0, 0), glm::quat());

	//create Upper fingers
	thumbUpper = physicsFactory->CreateCapsule(0.5f, fingerUpperHeight, glm::vec3(0, 0, 0), glm::quat());
	indexUpper = physicsFactory->CreateCapsule(0.5f, fingerUpperHeight, glm::vec3(0, 0, 0), glm::quat());


	//create joints to link fingers together
	//thumbLower to thumbUpper
	ptpConstraint= new btPoint2PointConstraint(*thumbLower->rigidBody, *thumbUpper->rigidBody, btVector3(0, fingerLowerHeight/2 + 1, 0), btVector3(0, -fingerUpperHeight/2, 0));
	dynamicsWorld->addConstraint(ptpConstraint);
	//indexLower to indexUpper
	ptpConstraint= new btPoint2PointConstraint(*indexLower->rigidBody, *indexUpper->rigidBody, btVector3(0, fingerLowerHeight/2 + 1, 0), btVector3(0, -fingerUpperHeight/2, 0));
	dynamicsWorld->addConstraint(ptpConstraint);


	//link fingers to palm
	//palm to thumbLower
	ptpConstraint= new btPoint2PointConstraint(*palm->rigidBody, *thumbLower->rigidBody, btVector3(-6, 0, -3), btVector3(0, -fingerLowerHeight/2, 0));
	dynamicsWorld->addConstraint(ptpConstraint);
	//palm to indexLower
	ptpConstraint= new btPoint2PointConstraint(*palm->rigidBody, *indexLower->rigidBody, btVector3(-3, 0, -5),btVector3(0,-fingerLowerHeight/2,0));
    dynamicsWorld->addConstraint(ptpConstraint);

	
	//create Finger tips
	thumbTip = physicsFactory->CreateKinematicSphere(1, glm::vec3(-6, 40, -7));
	indexTip = physicsFactory->CreateKinematicSphere(1, glm::vec3(-3, 40, -9));


	//thumbUpper to thumb tip
	ptpConstraint= new btPoint2PointConstraint(*thumbUpper->rigidBody, *thumbTip->rigidBody, btVector3(0, fingerUpperHeight/2, 0), btVector3(0, 0, 2));
    dynamicsWorld->addConstraint(ptpConstraint);
	//indexthumbUpper to index tip
	ptpConstraint= new btPoint2PointConstraint(*indexUpper->rigidBody, *indexTip->rigidBody, btVector3(0, fingerUpperHeight/2, 0),btVector3(0, 0, 2));
    dynamicsWorld->addConstraint(ptpConstraint);
	*/
	return true;
}

Hand::~Hand(void)
{
}

void Hand::Update(float timeDelta) 
{

	if(leapmotionController.isConnected())
	{
		Leap::Frame fNew = leapmotionController.frame();
		Leap::Frame fOld = leapmotionController.frame(1);

		Leap::Vector palmPosOld = Leap::Vector(palm->parent->position.x, palm->parent->position.y, palm->parent->position.z);

		Leap::Vector palmPosNew = fNew.hands()[0].palmPosition();

		Leap::Vector changeInPalmPos =  palmPosNew - palmPosOld;

		palm->parent->position = glm::vec3(palmPosNew.x, palmPosNew.y - 200, palmPosNew.z);

		//thumbTip->parent->position += glm::vec3(changeInPalmPos.x,changeInPalmPos.y-200,changeInPalmPos.z);
		//indexTip->parent->position += glm::vec3(changeInPalmPos.x,changeInPalmPos.y-200,changeInPalmPos.z);

		Leap::Hand h = fNew.hands()[0];

		float pitch = h.direction().pitch();
        float yaw = h.direction().yaw();
        float roll = h.palmNormal().roll();

		//palm->parent->Pitch(glm::degrees(pitch));
		//palm->parent->Yaw(glm::degrees(yaw));
		//palm->parent->Roll(glm::degrees(roll));

		Game::Instance()->PrintFloat("yaw " , glm::degrees(pitch));
		Game::Instance()->PrintFloat("pitch " , glm::degrees(yaw));
		Game::Instance()->PrintFloat("roll " , glm::degrees(roll));

		glm::vec3 aaa = glm::vec3(h.direction().x,h.direction().y,h.direction().z);
		glm::vec3 bbb = glm::vec3(h.palmNormal().x,h.palmNormal().y,h.palmNormal().z);
		bbb = glm::normalize(bbb);
		glm::vec3 axis = glm::cross(GameComponent::basisLook, aaa);
		axis = glm::normalize(axis);

		float theta = glm::acos(glm::dot(bbb, GameComponent::basisLook));
		palm->parent->orientation = glm::angleAxis(glm::degrees(theta), axis);


		/*
		Leap::FingerList fingerListNow = fNew.hands()[0].fingers();

		std::vector<Leap::Vector> fingerPositionsOld;
		std::vector<Leap::Vector> fingerPositions;

		for(int f = 0; f < fingerListNow.count(); f++)
		{
			Leap::Vector fp = fingerListNow[f].tipPosition();
			//fingerPositions.push_back(glm::vec3(fp.x ,fp.y - 200,fp.z ));
			fingerPositions.push_back(fp);

			if(fOld.hands()[0].fingers()[f].tipPosition() == Leap::Vector(0,0,0))
			{
				fingerPositionsOld.push_back(fp);
			}
			else
			{
				fingerPositionsOld.push_back(fOld.hands()[0].fingers()[f].tipPosition());
			}
			
		}
		
		std::sort(fingerPositions.begin(),fingerPositions.end(),XSmaller());
		std::sort(fingerPositionsOld.begin(),fingerPositionsOld.end(),XSmaller());
		int index = 0;

		if(fingerPositions.size() > 0)
		{
			Leap::Vector ch =  fingerPositions[0] - fingerPositionsOld[0];
			thumbTip->parent->position += glm::vec3(ch.x,ch.y,ch.z);
			index ++;
		}
		if(fingerPositions.size() > 1)
		{
			//indexTip->parent->position = fingerPositions[1];
			//Leap::Vector ol = Leap::Vector(indexTip->parent->position.x,indexTip->parent->position.y,indexTip->parent->position.z);
			//Leap::Vector ch = fingerPositions[1] - ol;
			Leap::Vector ch =   fingerPositions[1] - fingerPositionsOld[1];
			indexTip->parent->position += glm::vec3(ch.x,ch.y,ch.z);
			index ++;
		}
		if(fingerPositions.size() > 2)
		{
			Leap::Vector ch =   fingerPositions[2] - fingerPositionsOld[2];
			middleTip->parent->position += glm::vec3(ch.x,ch.y,ch.z);
			index ++;
		}
		if(fingerPositions.size() > 3)
		{
			Leap::Vector ch =   fingerPositions[3] - fingerPositionsOld[3];
			ringTip->parent->position += glm::vec3(ch.x,ch.y,ch.z);
			index ++;
		}
		if(fingerPositions.size() > 4)
		{
			Leap::Vector ch =   fingerPositions[4] - fingerPositionsOld[4];
			pinkyTip->parent->position += glm::vec3(ch.x,ch.y,ch.z);
			index ++;
		}

		*/

	}
	GameComponent::Update(timeDelta);
}
