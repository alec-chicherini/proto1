/*
Chemical experiment prototype engine
GitHub:		https://github.com/ikvasir/proto1
License: MIT
*/
#pragma once
//after init in main function generating vector random int value of RAND_ARRAY_SIZE  
//every next using of RAND or RAND100 moving iRand and iRand100 +1 position
//when vector is over it acces started from the first element
#include <random>
#include <vector>

#define RAND random_generator::get_rand()

//#define RAND_ARRAY_SIZE 10000

class random_generator
{

public:
     void rand_init(const long long RAND_ARRAY_SIZE) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(1, INT_MAX);

        vRand.clear();

        for (int i = 0; i < RAND_ARRAY_SIZE; ++i)
            vRand.push_back(dist(gen));

    }
  

    static int get_rand()
    {
        if (iRand == vRand.size() - 1)
        {
            iRand = 0;
            return vRand[vRand.size() - 1];
        }
        else
            return vRand[iRand++];
    }

   
    static std::vector<int> vRand;
    static int iRand;

   

   
};