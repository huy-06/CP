#include "../../../ds/num/mod/combinatorics.hpp"

#ifndef CP_ALG_LAGRANGE
#define CP_ALG_LAGRANGE
namespace cp {
namespace alg {

// 1. Nội suy Lagrange cho tập x và y bất kỳ. Độ phức tạp: O(N^2)
template <typename Tp>
Tp lagrange(const std::vector<Tp>& x, const std::vector<Tp>& y, const Tp& val) {
    int n = (int)x.size();
    assert(n == (int)y.size());
    Tp ans = 0;
    for (int i = 0; i < n; ++i) {
        Tp num = 1, den = 1;
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                num *= (val - x[j]);
                den *= (x[i] - x[j]);
            }
        }
        ans += y[i] * num * den.inv();
    }
    return ans;
}

// 2. Nội suy nhanh f(val) cho array y tuần tự với x_i = i {0, 1, ..., y.size() - 1}. Độ phức tạp: O(N)
template <typename Tp>
Tp lagrange(const std::vector<Tp>& y, const Tp& val) {
    int n = (int)y.size();
    if (n == 0) return Tp(0);
    
    static cp::ds::combinatorics<Tp> comb;
    std::vector<Tp> pref(n), suff(n);
    
    pref[0] = val;
    for (int i = 1; i < n; ++i) {
        pref[i] = pref[i - 1] * (val - Tp(i));
    }
    
    suff[n - 1] = val - Tp(n - 1);
    for (int i = n - 2; i >= 0; --i) {
        suff[i] = suff[i + 1] * (val - Tp(i));
    }
    
    Tp ans = 0;
    for (int i = 0; i < n; ++i) {
        Tp num = (i == 0 ? Tp(1) : pref[i - 1]) * (i == n - 1 ? Tp(1) : suff[i + 1]);
        Tp den = comb.inv_fact(i) * comb.inv_fact(n - 1 - i);
        
        if ((n - 1 - i) % 2 != 0) {
            den = -den;
        }
        
        ans += y[i] * num * den;
    }
    return ans;
}

} // namespace alg
} // namespace cp

#endif