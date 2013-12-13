#include "ivanPatri.h"
#include "Utils.h"

using namespace BGE;

ivanPatri::ivanPatri(void):numParticles(1000)
{
	
}

ivanPatri::ivanPatri(int numParticles)
{
	this->numParticles = numParticles;
	diffuse = glm::vec3(0,0,1);
	elapsed = 0;
}


ivanPatri::~ivanPatri(void)
{
}

bool ivanPatri::Initialise()
{
	for (int i = 0 ; i < numParticles ; i ++)
	{
		Particle p;
		InitParticle(p);
		particles.push_back(p);
	}
	return ParticleEffect::Initialise();
}

void ivanPatri::InitParticle(Particle & p)
{
	float radius = 0.25f;
	p.position = position;

	p.velocity = glm::vec3(RandomClamped(-radius, radius), RandomClamped(), RandomClamped(-radius, radius)) * 10.0f;
	p.velocity.y = glm::abs<float>(p.velocity.y);
	p.diffuse.r = diffuse.r * RandomClamped(0, 1);
	p.diffuse.g = diffuse.g * RandomClamped(0, 1);
	p.diffuse.b = diffuse.b * RandomClamped(0, 1);
	p.age = 0;
	p.alive = true;
	p.size = RandomClamped(10, 50);
	p.lifetime = 1.0f;
}

void ivanPatri::UpdateParticle(float timeDelta, Particle & p)
{
	static glm::vec3 gravity = glm::vec3(0, 0, 5);
	
	p.velocity += gravity * timeDelta;
	p.position += p.velocity * timeDelta;

	// Fade the alpha as we approach the ground
	float fadeHeight = 5;
	
	p.diffuse.a = glm::clamp<float>(p.position.y / fadeHeight, 0.0f, 1.0f);
	
	/*
	if (p.position.y < 0)
	{
		InitParticle(p);
	}
	*/
	//
	//if (p.position.y < position.y-10)
	//{
	//	InitParticle(p);
	//}
	
	
	float timeToPass = 1.0f / 3;

	if(elapsed > timeToPass)
	{
		InitParticle(p);
		elapsed = 0;
	}
	else
	{
		elapsed += timeDelta;
	}
	

}

void ivanPatri::Update(float timeDelta)
{
	ParticleEffect::Update(timeDelta);
}

