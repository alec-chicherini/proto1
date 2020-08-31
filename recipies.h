/*
Chemical experiment prototype engine
GitHub:		https://github.com/ikvasir/proto1
License: MIT
*/
#pragma once
#include "molecules.h"
#include "utilites.h"
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <tuple>
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


	  // 2Al + 6H20 = 2Al(OH)+3H2
	  // int32_t string + int32_t string = string
      // 2       Al     + 6       H20    = 2Al(OH)+3H2
	 
	  static std::vector<std::tuple<int32_t,std::string,int32_t,std::string,std::string>> RECIPIES;

	//parse csv file to RECIPIES variable
	csv_to_RECIPIES(std::string  FILE);
	csv_to_RECIPIES();


	void print_RECIPIES();
};