#include <vector>
#include <utility>

#ifndef CP_DS_MODULAR_COMBINATORICS
#define CP_DS_MODULAR_COMBINATORICS
namespace cp {
namespace ds {

template <typename Tp, typename Enable = void>
class combinatorics {
public:
    using value_type = Tp;

    combinatorics() : num{0}, facts{1} {}

    combinatorics(int n) : num{0}, facts{1} { init(n); }

    value_type fact(int n) {
        if (n > num) init(n << 1);
        return facts[n];
    }

    value_type C(int n, int k) {
        if (k < 0 || k > n) return value_type(0);
        return fact(n) / (fact(k) * fact(n - k));
    }

    value_type A(int n, int k) {
        if (k < 0 || k > n) return value_type(0);
        return fact(n) / fact(n - k);
    }

private:
    int num;
    std::vector<value_type> facts;

    void init(int n) {
        int old_num = num;
        if (n <= old_num) return;
        
        facts.resize(n + 1);
        if (old_num == 0) facts[0] = value_type(1);

        for (int i = old_num + 1; i <= n; ++i) {
            facts[i] = facts[i - 1] * value_type(i);
        }
        num = n;
    }
};

template <typename Tp>
class combinatorics<Tp, decltype((void)std::declval<Tp>().inv(), void())> {
public:
    using value_type = Tp;

    combinatorics() : num{0}, facts{1}, inv_facts{1}, invs{1} {
        facts[0] = 1;
        inv_facts[0] = 1;
        invs[0] = 0;
    }

    combinatorics(int n) : combinatorics() {
        init(n);
    }

    value_type fact(int n) {
        if (n > num) init(n << 1);
        return facts[n];
    }

    value_type inv_fact(int n) {
        if (n > num) init(n << 1);
        return inv_facts[n];
    }
    
    value_type inv(int n) {
        if (n > num) init(n << 1);
        return invs[n];
    }

    value_type C(int n, int k) {
        if (k < 0 || k > n) return 0;
        return fact(n) * inv_fact(k) * inv_fact(n - k);
    }

    value_type A(int n, int k) {
        if (k < 0 || k > n) return 0;
        return fact(n) * inv_fact(n - k);
    }

    value_type H(int n, int k) {
        if (n < 0 || k < 0) return 0;
        if (n == 0 && k == 0) return 1;
        return C(n + k - 1, k);
    }

    value_type catalan(int n) {
        return C(n << 1, n) * inv(n + 1);
    }

private:
    int num;
    std::vector<value_type> facts;
    std::vector<value_type> inv_facts;
    std::vector<value_type> invs;

    void init(int n) {
        int old_num = num;
        if (n <= old_num) return;

        facts.resize(n + 1);
        inv_facts.resize(n + 1);
        invs.resize(n + 1);

        if (old_num == 0) facts[0] = 1;
        for (int i = old_num + 1; i <= n; ++i) {
            facts[i] = facts[i - 1] * value_type(i);
        }

        inv_facts[n] = facts[n].inv();
        for (int i = n; i > old_num + 1; --i) {
            inv_facts[i - 1] = inv_facts[i] * value_type(i);
            invs[i] = inv_facts[i] * facts[i - 1];
        }

        if (n > old_num) {
            invs[old_num + 1] = inv_facts[old_num + 1] * facts[old_num];
        }

        num = n;
    }
};
} // namespace ds
} // namespace cp
#endif
