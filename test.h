#pragma once

//#define IN_CONSOLE

//#define TEST


//adds ne screen under main to check collisions
//#define SHOW_TEST_INFO

#ifdef SHOW_TEST_INFO
	#define SCREEN_SIZE_X 1280
	#define SCREEN_SIZE_Y 1000
	#define TARGET_SCREEN_SIZE_X 1280
	#define TARGET_SCREEN_SIZE_Y 720
#endif

#ifndef SHOW_TEST_INFO
	#define SCREEN_SIZE_X 1280
	#define SCREEN_SIZE_Y 720
	#define TARGET_SCREEN_SIZE_X SCREEN_SIZE_X
	#define TARGET_SCREEN_SIZE_Y SCREEN_SIZE_Y
#endif

#include <iostream>