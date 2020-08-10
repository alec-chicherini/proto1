#include "experiment_engine.h"
#include "test.h"
material theExperimentEngine::makeExperiment(material& mat1, material& mat2)
{

	material res{};
	
	int smaller = std::min(mat1.size(), mat2.size());
	int bigger = std::max(mat1.size(), mat2.size());

#ifdef TEST
	std::cout << " smaller= " << smaller << " bigger = " << bigger << std::endl;
#endif
	//copiing strings from two vector materials to manipulate(delete moleculas when operation happen)
	//with random experiment environment

	std::vector<std::string> usedMolSmaller;
	std::vector<std::string> usedMolBigger;
	if (smaller == mat1.size()) {
		usedMolSmaller = mat1.to_vector_string_mol();
		usedMolBigger = mat2.to_vector_string_mol();
	}
	else {
		usedMolSmaller = mat2.to_vector_string_mol();
		usedMolBigger = mat1.to_vector_string_mol();
	}

#ifdef TEST
	std::cout << "copying vectors start:" << std::endl;
	for (auto& s : usedMolSmaller)std::cout << s << " "; std::cout<<"<-smaller"<< std::endl;
	for (auto& s : usedMolBigger)std::cout << s << " "; std::cout <<"<-bigger"<< std::endl;
	std::cout << ":copying vectors finished" << std::endl;
#endif // TEST


	//auto random_check = [&usedMolSmaller, &usedMolBigger, &iCurrentSmall, &iCurrentBig,&smaller,&bigger](){};

		for (int i = 0; i < smaller; i++) {

			int iCurrentSmall=RAND % usedMolSmaller.size();
			std::string molFromSmall = usedMolSmaller[iCurrentSmall];
			auto iter = usedMolSmaller.begin();
			std::advance(iter, iCurrentSmall);
			usedMolSmaller.erase(iter);
		

			int iCurrentBig = RAND % usedMolBigger.size();
			std::string molFromBig = usedMolBigger[iCurrentBig];
			auto iter2 = usedMolBigger.begin();
			std::advance(iter2, iCurrentBig);
			usedMolBigger.erase(iter2);


#ifdef TEST
			//std::cout << "usedMolSmaller[" << iCurrentSmall << "] =" << molFromSmall << std::endl;
			//std::cout << "usedMolBigger[" << iCurrentBig << "] =" << molFromBig << std::endl;
			std::cout << i<<":|" << molFromSmall << "|" << " + " << "|" << molFromBig << "|" << " = ";

#endif // TEST
			molecules sum (molecules(molFromSmall) + molecules(molFromBig));
			//std::cout << "|" << (sum).get_name() << "|" << std::endl;
			if (sum.get_name() != "NULL"){
				res.add_1_mol(sum);

#ifdef TEST
				std::cout << "|" << (sum).get_name()<<"|"<<std::endl;
#endif // TEST
			}

			else {
				res.add_1_mol(molecules(molFromSmall));
				res.add_1_mol(molecules(molFromBig));
#ifdef TEST
				std::cout << "|" << molFromSmall << "|" << molFromBig << "|" << std::endl;
#endif // TEST
			}
		}//for
#ifdef TEST
		std::cout << "vectors sizes after main cycle of iteractions:" << std::endl;
		std::cout << "usedMolSmaller.size()="<< usedMolSmaller.size() << " usedMolBigger.size() = " << usedMolBigger.size() <<std::endl ;
		std::cout << "before adding last part: res.size() = " << res.size()<<std::endl;
#endif
		for (auto s : usedMolBigger)res.add_1_mol(molecules(s));

#ifdef TEST
		
		std::cout << "after adding last part: res.size() = " << res.size() << std::endl;
#endif
	return res;
}


