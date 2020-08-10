#include "molecules.h"
#include "recipies.h"
molecules::molecules(std::string _name)
{
	name = _name;
}

molecules::molecules(const molecules& mol)
{
	this->name = mol.name;
}


 molecules molecules::operator+(const molecules& mol2)
 {
	 
	 std::string sum_res((this->name) + (mol2.name));
#ifdef TEST
	// std::cout << " operator+ -> (this->name)+(mol2.name) = " << "|" << sum_res << "|" << std::endl;
#endif
	auto search = csv_to_RECIPIES::RECIPIES.find(sum_res);
	if(search != csv_to_RECIPIES::RECIPIES.end())
		return search->second;
		
	else 
		return molecules(std::string("NULL"));
		
	
}
