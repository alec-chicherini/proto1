#pragma once
#include <string>


class molecules {

private:
	
	std::string name;
	//bool stable;

public:

	molecules(std::string _name);
	molecules(const molecules& mol);
	

	std::string get_name() const { return this->name; }

	 molecules operator+(const molecules& mol2);
	
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