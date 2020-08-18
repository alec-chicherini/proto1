#pragma once
#include <string>
#include <vector>
#include <string>
#include <map>
#include <initializer_list>



class molecules {

private:
	
	std::string name;
	uint8_t radius=40;
	uint8_t R, G, B;
	
	//bool stable;

public:

	molecules(std::string _name);
	molecules(const molecules& mol);
	molecules();
	

	std::string get_name() const { return this->name; }
	int32_t get_radius() const { return this->radius; }
	auto get_color() const { return std::vector<uint8_t>{this->R,this->G,this->B,1}; }
	std::vector<molecules>  operator+(const molecules& mol2);

	bool is_recepie_with(const molecules& mol2);

	static std::map<std::string, std::vector<int32_t>> MOLS;

	//parse csv file to MOLS variable
	void csv_to_MOLS(std::string FILE);
	
	void print_MOLS();
	
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