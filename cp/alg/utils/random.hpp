#include <chrono>
#include <random>

#ifndef CP_ALG_RANDOM
#define CP_ALG_RANDOM
namespace cp {
namespace alg {

std::mt19937 random(std::chrono::steady_clock::now().time_since_epoch().count());
std::mt19937_64 random64(std::chrono::steady_clock::now().time_since_epoch().count());

} // namespace alg
} // namespace cp
#endif