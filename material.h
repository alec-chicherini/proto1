/*
Chemical experiment prototype engine
GitHub:		https://github.com/ikvasir/proto1
License: MIT
*/
#pragma once
#include "molecules.h"
#include <string>
#include <deque>
#include "experiment_enums.h"
#include <vector>

#ifndef NUMMOLMAT
	#define NUMMOLMAT 20
#endif



class material

{
private:
	
	//int weight;
	std::deque<molecules> structure;

public:


	material(molecules mol, MATERIAL_QUALITY purity);
	material(molecules mol, MATERIAL_QUALITY purity, int quantity);
	//material(molecules mol1, int quantity1, molecules mol2, int quantity2);
	//material(molecules mol);
	material();

	//connvert all molecules in material to one string
	std::string printMaterial();

	//number of molecules in matitial;
	int size();

	//adding one moleculas to mat
	void add_1_mol(molecules mol);

	//access i element in set.. for O(i)
	molecules operator[](int i);

	

	//convert material to vector<string>  names of molecules.
	std::vector<std::string> to_vector_string_mol();

};
