#include "Player.h"

using namespace BGE;

Player::Player(btDiscreteDynamicsWorld * dynamicsWorld)
{
	this->dynamicsWorld = dynamicsWorld;

	shared_ptr<PhysicsFactory> physicsFactory = make_shared<PhysicsFactory>(dynamicsWorld);
	// Now some objects
	shared_ptr<PhysicsController> box1 = physicsFactory->CreateBox(1,1,4, glm::vec3(5, 5, 0), glm::quat()); 
	shared_ptr<PhysicsController> box2 = physicsFactory->CreateBox(1,1,4, glm::vec3(5, 5, 5), glm::quat()); 

}


Player::~Player(void)
{
}

void Player::Update(float timeDelta) 
{

}
