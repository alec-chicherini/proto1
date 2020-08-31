/*
Chemical experiment prototype engine
GitHub:		https://github.com/ikvasir/proto1
License: MIT
*/
#pragma once
#include <string>
#include <vector>
#include <string>
#include <map>
#include "defined_globals.h"
#include "random_generator.h"
#include "test.h"
#include "recipies.h"
#include "utilites.h"



class molecules {

private:
	
	std::string name;
	int32_t radius;
	int32_t R, G, B;
	int32_t lifetime;
	
	//bool stable;

public:
	friend class csv_to_RECIPIES;
	molecules(std::string _name);
	molecules(const molecules& mol);
	molecules();
	

	std::string get_name() const { return this->name; }
	int32_t get_radius() const { return this->radius; }
	auto get_color() const { return std::vector<int32_t>{this->R,this->G,this->B,1}; }
	auto get_lifetime()const { return this->lifetime; }
	
	std::vector<molecules>  operator+(const molecules& mol2);
	std::vector<molecules>  decay() ;

	std::vector<std::string> is_recepie_with(const molecules mol2);
	bool is_iterract(const molecules mol2);

	static std::map<std::string, std::vector<int32_t>> MOLS;

	//parse csv file to MOLS variable
	void csv_to_MOLS(std::string FILE);
	
	void print_MOLS();
	//moved to utilty.h
	//void parseMol(std::string& name, int32_t& num);
	// parseMol(std::string& name);


};

/*
namespace std
{
	template<> struct less<molecules>
	{
		bool operator() (const molecules& lhs,  const molecules& rhs) const
		{
			return lhs.get_name() < rhs.get_name();
		}
	};
};*/