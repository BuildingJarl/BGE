#pragma once
#include "GameComponent.h"
#include "Leap.h"
#include "Game.h"
#include <btBulletDynamicsCommon.h>

namespace BGE 
{
	class Hand:
		public GameComponent
	{
	private:
		Leap::Controller leapmotionController;
		btDiscreteDynamicsWorld * dynamicsWorld;

		shared_ptr<GameComponent> CreatePalm(float palmRadius,float palmHeight, glm::vec3 palmPos, glm::quat quat);
		shared_ptr<GameComponent> CreateFinger(float fingerHeight,float fingerRadius, glm::vec3 fingerPostion, glm::quat quat);
		shared_ptr<GameComponent> CreateCapsule(float height, float radius, glm::vec3 pos, glm::quat quat);

		void CreateHand(glm::vec3 pos,glm::quat quat);
		
		shared_ptr<GameComponent> palm;

	public:
		Hand(Leap::Controller leapmotionController, btDiscreteDynamicsWorld * dynamicsWorld);
		~Hand(void);
		void Update(float timeDelta);
		
	};

}


