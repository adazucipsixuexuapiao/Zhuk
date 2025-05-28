#include "random.h"

// seeds by time for main menu
std::mt19937 r_engine(std::time(0));

void seed_engine(unsigned int _seed)
{r_engine.seed(_seed);}