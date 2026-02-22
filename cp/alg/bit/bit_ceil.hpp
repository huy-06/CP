#ifndef CP_ALG_BIT_CEIL
#define CP_ALG_BIT_CEIL
namespace cp {

namespace internal {

constexpr unsigned int bit_ceil(unsigned int n, int shift) {
    return shift == 32 ? n + 1 : bit_ceil(n | (n >> shift), shift << 1);
}

} // namespace internal

namespace alg {

constexpr unsigned int bit_ceil(unsigned int n) {
    return n <= 1 ? 1 : internal::bit_ceil(n - 1, 1);
}

} // namespace alg
} // namespace cp
#endif
