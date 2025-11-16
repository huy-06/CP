#include <vector>

#ifndef CP_ALG_DIVISOR
#define CP_ALG_DIVISOR
namespace cp {
namespace alg {

template<typename Tp>
constexpr std::vector<Tp> divisors(Tp n) {
    std::vector<Tp> a, b;
    for (Tp i = 1; i * i <= n; ++i) {
        if (n % i == 0) {
            a.push_back(i);
            b.push_back(n / i);
        }
    }
    if (a.back() == b.back()) b.pop_back();
    a.insert(a.end(), b.rbegin(), b.rend());
    return a;
}

} // namespace alg
} // namespace cp
#endif