#pragma once

//#define IN_CONSOLE

#define TEST


//adds ne screen under main to check collisions
#define SHOW_COLLISION_ARRAY

#ifdef SHOW_COLLISION_ARRAY
	#define SCREEN_SIZE_X 1640
	#define SCREEN_SIZE_Y 1000
#endif

#ifndef SHOW_COLLISION_ARRAY
#define SCREEN_SIZE_X 640
#define SCREEN_SIZE_Y 480
#endif

#include <iostream>