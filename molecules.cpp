/*
Chemical experiment prototype engine 
GitHub:		https://github.com/ikvasir/proto1
License: MIT
*/
#include "molecules.h"

molecules::molecules(std::string _name)
{
	
	name = _name;
	
	auto str = _name;
	int32_t num;

	parseMol(str, num);

	if(num == 0)std::cout<<"ERROR -> zero number of molls passed to ctor: " +_name;

	if(num==1)
	{
	if (_name[0] == '1')_name.erase(0, 1);
	auto search = molecules::MOLS.find(str);
	if (search != molecules::MOLS.end())
		{
		radius = search->second[0];
		R = search->second[1];
		G = search->second[2];
		B = search->second[3];
		lifetime = search->second[4];
		}

	else {
		radius = 20;
		R = 0;
		G = 0;
		B = 0;
		lifetime = -1;
		}
	}
	else //if number of molecules more than 1.
	{
		auto searchX = molecules::MOLS.find("*"+str);
		auto search = molecules::MOLS.find(str);
		if ((search != molecules::MOLS.end())&& 
			(searchX != molecules::MOLS.end()))
		{
			//values of molecules depends on number of mols. Growing values consists in *H20.
			radius = search->second[0]+searchX->second[0]*num;
			if (radius > MAX_MOL_RADIUS)radius = MAX_MOL_RADIUS;
			R = (search->second[1]+ searchX->second[1])/2;
			G = (search->second[2]+ searchX->second[2])/2;
			B = (search->second[3]+ search->second[3])/2;
			lifetime = std::round((searchX->second[4]) *0.05f*num);
			if (lifetime < 3)lifetime = 3;
		}
		else {
			radius = 20;
			R = 0;
			G = 0;
			B = 0;
			lifetime = -1;
		}
	
	}
}

molecules::molecules(const molecules& mol)
{
	this->name = mol.name;
	this->radius = mol.radius;
	this->R = mol.R;
	this->G = mol.G;
	this->B = mol.B;
	this->lifetime = mol.lifetime;
}

molecules::molecules() {
	this->R = 0;
	this->G = 0;
	this->B = 0;
	this->radius = MAX_MOL_RADIUS;
	this->lifetime = -1;
};


 std::vector<molecules> molecules::operator+(const molecules& mol2)
 {

	 std::vector<molecules> res;
	 
	 auto vec = this->is_recepie_with(mol2);
	 auto str = vec[0];



	if (str!="NULL")
	{


		auto smartPush = [&res](std::string Name, int32_t Num)
		{
			if (Num == 0)return false;
			else if (Num > 1)
			{
				for (int i = 0; i < Num; i++)
					res.push_back(molecules(Name));
			}
			else res.push_back(molecules(Name));
			return true;
		};



#ifdef  TEST
		std::cout << "str = this->is_recepie_with(mol2)[0]; = " << str << std::endl;
		
#endif //  TEST

		//number of "+"s in string, number of different molecules to output
		int pls = std::count(str.begin(), str.end(), '+');

#ifdef  TEST
		std::cout << "pls = " << pls << std::endl;

#endif //  TEST

		//first=0, last possition of curent substring
		int last;
		if (pls == 0)last = str.end() - str.begin();
			else last = str.find_first_of('+');

		for(int i = 0;i<pls+1;i++)
		{

			std::string Name = str.substr(0, last);
			int32_t Num;
			parseMol(Name, Num);

#ifdef  TEST
			std::cout << " str =|" << str << "|" ;
			std::cout << " i = " << i ;
			std::cout << " last = " << last 
				<< " Name = " << Name
				<< " Num = " << Num
				<< std::endl;
			//std::cout << "i = " << i << std::endl;
#endif //  TEST

			smartPush(Name, Num);
		
			if (pls > 0)
			{
				str = str.substr(str.find_first_of("+") + 1, str.end() - str.begin() - str.find_first_of("+") + 1);

				if (pls > i+1)last = str.find_first_of("+");
					else last = str.end() - str.begin();
					
			};

		}

	//if resuld additional mols not zeros
		std::string Name1 = vec[1];
		int32_t Num1;
		parseMol(Name1, Num1);

		smartPush(Name1, Num1);

		std::string Name2 = vec[2];
		int32_t Num2;
		parseMol(Name2, Num2);

		smartPush(Name2, Num2);

#ifdef  TEST
		std::cout << "operator+::res.size() = " << res.size()<<std::endl;
		for (auto& r : res)
		{
			std::cout << r.get_name() << "|";
		}
		std::cout << std::endl;
#endif //  TEST



		return res;
	}
		
	else {

		//parse molecules for making operrations like H20+H20 = 2H20
		std::string firstName=this->name, 
					secondName=mol2.name;
		int32_t firstNum(0), secondNum(0);

		//max mol size = 99999H20
		parseMol(firstName, firstNum);
		parseMol(secondName, secondNum);


#ifdef TEST
		std::cout << "PARSE MOL RESULT::" << "this->name=|" << this->name <<"|"<< std::endl;
		std::cout << "PARSE MOL RESULT::" << "firstName=|" << firstName << "|firstNum=|" << firstNum <<"|" << std::endl;
		std::cout << "PARSE MOL RESULT::" << "mol2.name=|" << mol2.name <<"|" << std::endl;
		std::cout << "PARSE MOL RESULT::" << "secondName=|" << secondName << "|secondNum=|" << secondNum <<"|" << std::endl;
#endif

		if(firstName==secondName)
		res.push_back(molecules(std::to_string(firstNum + secondNum) + firstName));
		else{
		//if comparision didnt find return empty vec
		//res.push_back(molecules(std::string("NULL")));
#ifdef  TEST
		std::cout << "operator+::res NULL Vector returned:  " << res.size() << std::endl;
		throw("operator+::res NULL Vector returned: ");

#endif //  TEST
		}
		return res;
	}	
		
	
}

std::vector<molecules> molecules::decay()
{
	std::vector<molecules> res;

	std::string str=this->name;
	int32_t num;

	parseMol(str, num);
	auto total = num;

	//res.push_back(molecules(str));
	//res.push_back(molecules(std::to_string(num - 1) + str));

	//decay to random number of parts
	auto parts = (RAND % 5);
	if (parts > num)parts = num;
	if (parts < 2)parts = 2;

	for(int i=0;i<parts;i++)
		{ 
		auto thisPart = num / parts;
		if(thisPart == 1)
		res.push_back(molecules(str));
		else
		res.push_back(molecules(std::to_string(thisPart) + str));
		total -= thisPart;

		if ((i == (parts - 1)) && total != 0)
			if (thisPart == 1)
				res.push_back(molecules(str));
			else if (total == 1)
				res.push_back(molecules(str));
			else res.push_back(molecules(std::to_string(total) + str));
		}

#ifdef TEST

	std::cout << "molecules::decay()::name()-> " << this->name << " decaying to:|";
	std::string msg2 = "";

	for (auto &r : res) { msg2 += r.name; msg2 += "|"; }
	
	std::cout <<msg2<< std::endl;

#endif TEST



	std::string msg="";
	
	for (auto &r : res) { msg += r.name; msg += "+"; }
	if (msg.back() == '+') msg.pop_back();
	std::string resultMsg = std::string(this->name+ std::string(" = ")+ msg);
	IterLog log;
	log.addIteractionLogMsg(resultMsg,"");
	//log.addIteractionLogMsg(123);

	return res;
}

bool molecules::is_iterract(const molecules mol2) 
{
	std::string firstName = this->name,
		        secondName = mol2.name;

	int32_t firstNum, secondNum;

	parseMol(firstName);
	parseMol(secondName);

	if (firstName == secondName)return true;

	else if (this->is_recepie_with(mol2)[0] != "NULL") return true;

	else return false;
		
}

//return vector of string with result, first moll overhead,second mol overhead.
 std::vector<std::string> molecules::is_recepie_with(const molecules mol2)
 {
	 std::vector<std::string> res;
	
	 std::string firstName = this->name,
				 secondName = mol2.name;

	 int32_t firstNum, secondNum;

	 parseMol(firstName,firstNum);
	 parseMol(secondName,secondNum);

		 for (auto& r : csv_to_RECIPIES::RECIPIES)
		 {

			 if (firstName == std::get<1>(r) && secondName == std::get<3>(r))
				 if (firstNum >= std::get<0>(r) && secondNum >= std::get<2>(r))
				 {
					 res.push_back(std::get<4>(r));
					 res.push_back(std::to_string(firstNum-std::get<0>(r))+ std::get<1>(r));
					 res.push_back(std::to_string(secondNum - std::get<2>(r)) + std::get<3>(r));

#ifdef TESTT
					 std::cout << __FUNCTION__<<"res: ";
					 for (auto& r : res)
						 std::cout << r << "|";
					 std::cout << std::endl;

#endif

					 return res;
				 }
					
		     if (firstName == std::get<3>(r) && secondName == std::get<1>(r))
				 if (firstNum >= std::get<2>(r) && secondNum >= std::get<0>(r))
				 {
					 res.push_back(std::get<4>(r));
					 res.push_back(std::to_string(firstNum - std::get<2>(r)) + std::get<3>(r));
					 res.push_back(std::to_string(secondNum - std::get<0>(r)) + std::get<1>(r));

#ifdef TESTT
					 std::cout << __FUNCTION__ << "res: ";
					 for (auto& r : res)
						 std::cout << r << "|";
					 std::cout << std::endl;

#endif
					 return res;
				 }
		 }
#ifdef TESTT
		 std::cout << __FUNCTION__ << "res: ";
		 for (auto& r : res)
			 std::cout << r << "|";
		 std::cout << std::endl;

#endif
		 return {"NULL","NULL","NULL"};
 }


 void molecules::csv_to_MOLS(std::string FILE)
 {
	 std::ifstream file;
	 file.open(FILE);
	 if (!file)
#ifdef TEST
		 std::cout << "File not oppened :" << FILE << std::endl;
#endif

#ifndef TEST
	 throw(std::exception("FILE WITH MOLS NOT OPPENED"));
#endif // !TEST
	 else {
		 std::string s;
		 for (file >> s; !file.eof(); file >> s) {
			 if (s[0] == '/' && s[1] == '/')continue;
#ifdef TEST
			// std::cout << s << std::endl;
#endif
			 std::string name;
			 int32_t rad;
			 int32_t r, g, b,lifetime;

			 auto first_comma = s.find_first_of(",");
			 name = s.substr(0, first_comma);

			 auto second_comma = s.find_first_of(",", first_comma+1);
			 rad = std::stoi(s.substr(first_comma + 1, second_comma - first_comma));

			 auto third_comma = s.find_first_of(",", second_comma+1);
			 r = std::stoi(s.substr(second_comma + 1, third_comma - second_comma));

			 auto fourth_comma = s.find_first_of(",", third_comma+1);
			 g = std::stoi(s.substr(third_comma + 1, fourth_comma - third_comma));

			 auto fifth_comma = s.find_first_of(",", fourth_comma + 1);
			 b = std::stoi(s.substr(fourth_comma + 1, fifth_comma - fourth_comma));

			 lifetime = std::stoi(s.substr(fifth_comma + 1, s.size() - fifth_comma));


#ifdef TEST
			 std::cout << name << ' ' << rad << ' ' << r << ' ' << g << ' ' << b << ' ' << lifetime << ' ' << std::endl;
#endif

			 //insert Mols

			 MOLS.insert({ name, {rad,r,g,b,lifetime} });
		 }

	 }
 }

 void molecules::print_MOLS()
 {
	 std::cout << "|MOLS:" << std::endl;
	 for (auto& m : MOLS)
	 {
		 std::cout << m.first << "|";
		 for (auto i = 0; i < m.second.size(); i++)
			 std::cout << m.second[i] << "|";
		 std::cout << std::endl;

	 }
	 std::cout << "|END OF MOLS|" << std::endl;
 }

 
 