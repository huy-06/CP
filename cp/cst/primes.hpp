#include <array>

#ifndef CP_CST_PRIMES
#define CP_CST_PRIMES
namespace cp {
namespace cst {

constexpr std::array<int, 12> small_primes = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};

constexpr std::array<int, 7> test_primes = {2, 325, 9375, 28178, 450775, 9780504, 1795265022};
    
} // namespace cst
} // namespace cp
#endif