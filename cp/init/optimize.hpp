#include "library.hpp"

#ifndef CP_OPTIMIZE
#define CP_OPTIMIZE

#pragma GCC optimize("O3,unroll-loops")
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")

#endif