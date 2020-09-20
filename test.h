/*
Chemical experiment prototype engine
GitHub:		https://github.com/ikvasir/proto1
License: MIT
*/
#pragma once

//#define IN_CONSOLE

//#define TEST_HARD_DEBUG

#define TEST

//adds ne screen under main to check collisions
#define SHOW_TEST_INFO

#ifdef SHOW_TEST_INFO
	#define SCREEN_SIZE_X 1280
	#define SCREEN_SIZE_Y 1000
	#define TARGET_SCREEN_SIZE_X 1280
	#define TARGET_SCREEN_SIZE_Y 720
#endif

#include <vector>
#include <string>
#include <array>

class IterLog{
	public:
static std::vector<std::string> iteractionsLog;

template<typename... Ts>
constexpr auto make_array(Ts&&... ts)
->std::array<std::string, sizeof...(Ts)>
{
	return { std::forward<Ts>(ts)... };
}


template<class ... Ts>
 constexpr void addIteractionLogMsg(Ts...msgs)
{
	constexpr int sz = sizeof...(msgs);

	std::array<std::string, sz> ceA = make_array(msgs...);
	
	std::string currentLogString = "";

	for (auto i = 0; i != sz - 1; i++)currentLogString += ceA[i];

	iteractionsLog.insert(iteractionsLog.begin(),currentLogString);

}

};





#ifndef SHOW_TEST_INFO
	#define SCREEN_SIZE_X 1280
	#define SCREEN_SIZE_Y 720
	#define TARGET_SCREEN_SIZE_X SCREEN_SIZE_X
	#define TARGET_SCREEN_SIZE_Y SCREEN_SIZE_Y
#endif

#include <iostream>