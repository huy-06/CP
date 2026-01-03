#if __cplusplus >= 202002L
#include <bit>
#endif

#ifndef CP_ALG_BIT_CEIL
#define CP_ALG_BIT_CEIL
namespace cp {
namespace alg {

#if __cplusplus >= 202002L

using std::bit_ceil;

#else

// @return same with std::bit::bit_ceil
unsigned int bit_ceil(unsigned int n) {
    unsigned int x = 1;
    while (x < n) {
        x <<= 1;
    }
    return x;
}

#endif

} // namespace alg
} // namespace cp
#endif
