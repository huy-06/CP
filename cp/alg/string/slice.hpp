#include <vector>
#include <string>

#ifndef CP_ALG_STRING_SLICE
#define CP_ALG_STRING_SLICE
namespace cp {
namespace alg {

template <typename Tp>
Tp slice(const Tp& a, int l, int r) {
    const int n = int(a.size());
    if (l < 0) l += n;
    if (r < 0) r += n;
    l = std::max(l, 0);
    r = std::min(r, n);
    if (l >= r) return Tp();
    return Tp(std::begin(a) + l, std::begin(a) + r);
}

} // namespace alg
} // namespace cp
#endif