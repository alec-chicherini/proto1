/*
Chemical experiment prototype engine
GitHub:		https://github.com/ikvasir/proto1
License: MIT
*/
#include "material.h"
#include "random_generator.h"
#include "test.h"

material::material(molecules mol, MATERIAL_QUALITY purity)
{
	for (int i = 0; i < NUMMOLMAT; i++)
		
	{
		int rnd = RAND;
		
		if ((rnd % 100) <= static_cast<int>(purity))
			structure.insert(structure.end(), mol);
					
		else structure.insert(structure.end(), molecules("H2O"));
						
		}

}

material::material(molecules mol, MATERIAL_QUALITY purity, int quantity)
{
	for (int i = 0; i < quantity; i++)

	{
		int rnd = RAND;

		if ((rnd % 100) <= static_cast<int>(purity))
			structure.insert(structure.end(), mol);

		else structure.insert(structure.end(), molecules("H2O"));

	}
}

material::material()
{
	structure = {};
}

std::string material::printMaterial()
{

#ifdef TEST
	std::cout << "material::printMaterial() - >structure.size() " << structure.size()<<std::endl;
#endif

std::string result{ "" };



	for (auto& mat : structure) {
		
		result += mat.get_name();
		result += ' ';
	}
	result += "\n";
	return result;
}

int material::size()
{
	return structure.size();
}

void material::add_1_mol(molecules mol)
{
	structure.insert(structure.end(),mol);
}


molecules material::operator[](int i)
{
	return structure[i];
}

std::vector<std::string> material::to_vector_string_mol()
{
	std::vector<std::string> vec;

	for (auto& s : structure) 
		vec.push_back(s.get_name());

		return std::move(vec);
}



