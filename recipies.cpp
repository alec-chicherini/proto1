#include "recipies.h"


csv_to_RECIPIES::csv_to_RECIPIES(std::string FILE)
{
    std::ifstream file;
    file.open(FILE);
    if (!file)
#ifdef TEST
        std::cout << "File not oppened :" << FILE << std::endl;
#endif

#ifndef TEST
    throw(std::exception("FILE WITH RECEPIES NOT OPPENED"));
#endif // !TEST
    else {
    std::string s;
    for (file >> s; !file.eof(); file >> s) {
#ifdef TEST
       // std::cout << s << std::endl;
#endif
        std::string s1, s2, s3;

        s1 = s.substr(0, s.find_first_of(","));
        s2 = s.substr(s.find_first_of(",") + 1, s.find_last_of(",") - s.find_first_of(",") - 1);
        s3 = s.substr(s.find_last_of(",") + 1, s.size() - s.find_last_of(","));


#ifdef TEST
       // std::cout << s1 << ' ' << s2 << ' ' << s3 << ' ' << std::endl;
#endif

        //insert recepie
        RECIPIES.insert({ s1 + s2,s3 });
        RECIPIES.insert({ s2 + s1,s3 });
    }
    };
}

csv_to_RECIPIES::csv_to_RECIPIES()
{
}

void csv_to_RECIPIES::print_RECIPIES()
{
    std::cout << "|RECEPIES:" << std::endl;
    for (auto& s : RECIPIES) std::cout << "|" << s.first << "|" << s.second << "|"<< std::endl;
    std::cout << "|END OF RECEPIES|" << std::endl;
}
