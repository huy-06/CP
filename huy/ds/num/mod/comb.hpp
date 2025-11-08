#include "static.hpp"

#ifndef HUY_DS_MOD_COMB
#define HUY_DS_MOD_COMB
namespace huy {
namespace ds {

template<typename Tp>
class comb {
public:
    comb() : num{0}, facts{1}, inv_facts{1}, invs{1} {}

    comb(int n) {
        init(n);
    }

    Tp fact(int n) {
        if (n > num) init(n << 1);
        return facts[n];
    }

    Tp inv_fact(int n) {
        if (n > num) init(n << 1);
        return inv_facts[n];
    }
    
    Tp inv(int n) {
        if (n > num) init(n << 1);
        return invs[n];
    }

    Tp C(int n, int k) {
        if (k < 0 || k > n) return 0;
        return fact(n) * inv_fact(k) * inv_fact(n - k);
    }

    Tp A(int n, int k) {
        if (k < 0 || k > n) return 0;
        return fact(n) * inv_fact(n - k);
    }

    Tp H(int n, int k) {
        if (n < 0 || k < 0) return 0;
        return C(n + k - 1, k);
    }

    Tp catalan(int n) {
        return C(n << 1, n) * inv(n + 1);
    }

private:
    int num;
    std::vector<Tp> facts;
    std::vector<Tp> inv_facts;
    std::vector<Tp> invs;

    void init(int n) {
        facts.resize(n + 1);
        inv_facts.resize(n + 1);
        invs.resize(n + 1);
        for (int i = num + 1; i <= n; ++i) {
            facts[i] = facts[i - 1] * Tp(i);
        }
        inv_facts[n] = facts[n].inv();
        for (int i = n; i > num; --i) {
            inv_facts[i - 1] = inv_facts[i] * Tp(i);
            invs[i] = inv_facts[i] * facts[i - 1];
        }
        num = n;
    }
};

} // namespace ds
} // namespace huy
#endif
