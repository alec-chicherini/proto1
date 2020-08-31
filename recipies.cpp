/*
Chemical experiment prototype engine
GitHub:		https://github.com/ikvasir/proto1
License: MIT
*/
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
        if (s[0] == '/' && s[1] == '/')continue;

        std::string s1, s2, s3;

        s1 = s.substr(0, s.find_first_of(","));
        s2 = s.substr(s.find_first_of(",") + 1, s.find_last_of(",") - s.find_first_of(",") - 1);
        s3 = s.substr(s.find_last_of(",") + 1, s.size() - s.find_last_of(","));


#ifdef TEST
       // std::cout << s1 << ' ' << s2 << ' ' << s3 << ' ' << std::endl;
#endif


        int32_t num1, num2;

        parseMol(s1, num1);
        parseMol(s2, num2);


        //insert recepie
        auto tup = std::make_tuple(num1, s1, num2, s2, s3);
        RECIPIES.push_back(tup);

    }
    };
}

csv_to_RECIPIES::csv_to_RECIPIES()
{
}

void csv_to_RECIPIES::print_RECIPIES()
{
    std::cout << "|RECEPIES:" << std::endl;
    for (auto& s : RECIPIES) std::cout << "|" << std::get<0>(s) << "|" << std::get<1>(s) << "|" << std::get<2>(s) << "|" << std::get<3>(s) << "|" << std::get<4>(s)<< std::endl;
    std::cout << "|END OF RECEPIES|" << std::endl;
}
