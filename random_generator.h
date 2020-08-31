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

#define RAND_W random_generator::get_rand_W()
#define RAND_X random_generator::get_rand_W()

#define RAND_H random_generator::get_rand_H()
#define RAND_Y random_generator::get_rand_H()

#define RAND100 random_generator::get_rand100() 
#define RAND_SIGN random_generator::get_rand_sign() 

#define RAND_ARRAY_SIZE 10000

class random_generator
{

public:
    void rand_init(int Width,int  Heidth){
    std::random_device rd;  
    std:: mt19937 gen(rd()); 
    std::uniform_int_distribution<> dist100(1, 100); 
    std::uniform_int_distribution<> dist(1, INT_MAX);
    std::uniform_int_distribution<> distH(1, Heidth);
    std::uniform_int_distribution<> distW(1, Width);
    std::uniform_int_distribution<> distRandSign(0, 1);

    vRand100.clear();
    vRand.clear();
    vRandH.clear();
    vRandW.clear();
    vRandSign.clear();

    for (int i = 0; i < RAND_ARRAY_SIZE; ++i)
    {
        vRand100.push_back(dist100(gen));
        vRand.push_back(dist(gen));
        vRandW.push_back(distW(gen));
        vRandH.push_back(distH(gen));
        if (distRandSign(gen))
            vRandSign.push_back(1);
        else(vRandSign.push_back(-1));
    } 
    }


   static int get_rand100() 
    {
    if (iRand100 == RAND_ARRAY_SIZE-1)
        {
            iRand100 = 0;
            return vRand100[RAND_ARRAY_SIZE-1];
        }
    else 
        return vRand100[iRand100++];
    }

    static int get_rand()
    {
        if (iRand == RAND_ARRAY_SIZE - 1)
        {
            iRand = 0;
            return vRand[RAND_ARRAY_SIZE - 1];
        }
        else
            return vRand[iRand++];
    }

    static int get_rand_H()
    {
        if (iRandH == RAND_ARRAY_SIZE - 1)
        {
            iRandH = 0;
            return vRandH[RAND_ARRAY_SIZE - 1];
        }
        else
            return vRandH[iRandH++];
    }

    static int get_rand_W()
    {
        if (iRandW == RAND_ARRAY_SIZE - 1)
        {
            iRandW = 0;
            return vRandW[RAND_ARRAY_SIZE - 1];
        }
        else
            return vRandW[iRandW++];
    }
    

    static int get_rand_sign()
    {
        if (iRandSign == RAND_ARRAY_SIZE - 1)
        {
            iRandSign = 0;
            return vRandSign[RAND_ARRAY_SIZE - 1];
        }
        else
            return vRandSign[iRandSign++];
    }


    static std::vector<int> vRand100;
    static int iRand100;

    static std::vector<int> vRand;
    static int iRand;

    static std::vector<int> vRandH;
    static int iRandH;

    static std::vector<int> vRandW;
    static int iRandW;

    static std::vector<int> vRandSign;
    static int iRandSign;

   
};