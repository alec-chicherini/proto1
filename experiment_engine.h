/*
Chemical experiment prototype engine
GitHub:		https://github.com/ikvasir/proto1
License: MIT
*/
#pragma once
#include <string>
#include "material.h"
#include "molecules.h"
#include "random_generator.h"
#include "experiment_enums.h"
#include "recipies.h"
#include <iterator>
#include <iostream>
#include <vector>

//testing class to making iterractions with molecules and materials and output to command window.
class theExperimentEngine
{
private: 

public:
	
	material makeExperiment(material& mat1, material& mat2);


	//void start();

};