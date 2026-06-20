#include <vector>
#include "factorize.hpp"

#ifndef CP_ALG_DIVISORS
#define CP_ALG_DIVISORS
namespace cp {
namespace alg {

template <typename Tp>
cp_constexpr std::vector<Tp> divisors(Tp n) {
    auto factors = factorize(x);
    std::vector<Tp> divisors;
    
    std::function<void(int, Tp)> dfs = [&](int idx, Tp val) -> void {
        if (idx == factors.size()) {
            divisors.push_back(val);
            return;
        }
        Tp p = factors[idx].first;
        Tp p_pow = 1;
        for (int i = 0; i <= factors[idx].second; ++i) {
            dfs(idx + 1, val * p_pow);
            if (i < factors[idx].second) 
                p_pow *= p;
        }
    };

    dfs(0, 1);
    sort(divisors.begin(), divisors.end());
    return divisors;
}

} // namespace alg
} // namespace cp
#endif