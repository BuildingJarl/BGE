#include<sdl.h>
#include <windows.h>
#include <GL/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include<glm.hpp>
#include "Game.h"
#include "Assignment.h"

using namespace BGE;

int main(int argc, char *argv[])
{
	
	//Lab5 game;

	// Uncomment this line to run the Rift/Connect/Physics demo as per 
	// http://www.youtube.com/watch?v=EEbVHxOkTxw
	Assignment game;


	game.Run();

	return 0;
}