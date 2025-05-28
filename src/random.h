#ifndef RANDOM_H
#define RANDOM_H

#include <random>
#include <ctime>

#define genrand(a,b) std::uniform_int_distribution<int>(a,b)(r_engine)
extern std::mt19937 r_engine;

void seed_engine(unsigned int _seed);

#define eng_grand(a,b,eng) std::uniform_int_distribution<int>(a,b)(eng)

#endif