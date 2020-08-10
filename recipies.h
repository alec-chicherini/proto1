#pragma once
#include <map>
#include <string>
#include <fstream>
#include "test.h"
//first string two strings sum. second result.
//H2 + S = H2S
//<H2S, H2S>

/*
=
{ 
	
};
*/




class csv_to_RECIPIES {
public:
	
	static std::map<std::string, std::string> RECIPIES;
	//parse csv file to RECIPIES variable
	csv_to_RECIPIES(std::string  FILE);

	void print_RECIPIES();
};