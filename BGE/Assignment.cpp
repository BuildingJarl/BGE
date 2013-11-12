#include "Assignment.h"


using namespace BGE;

Assignment::Assignment(void)
{
}

Assignment::~Assignment(void)
{
}


bool Assignment::Initialise()
{
	//std::shared_ptr<GameComponent> ground = make_shared<Ground>();
	//Attach(ground);


	//settings
	riftEnabled = false;
	fullscreen = false;

	Game::Initialise();

	return true;
}

void Assignment::Update(float timeDelta)
{

	Game::Update(timeDelta);
}