#include "../../../alg/math/nt/factorize.hpp"

#ifndef CP_DS_NUMBER_THEORY
#define CP_DS_NUMBER_THEORY
namespace cp {
namespace ds {

class number_theory {
public:
    number_theory(int limit = 10000000) : n(limit) {
        spf.resize(n + 1);
        std::iota(spf.begin(), spf.end(), 0); 
        
        for (int i = 2; i <= n; i++) {
            if (spf[i] == i) 
                primes.push_back(i);
            for (int p : primes) {
                if (p > spf[i] || 1LL * i * p > n) 
                    break;
                spf[i * p] = p;
            }
        }
    }

    template <typename Tp>
    bool is_prime(Tp x) const {
        if (x <= 1) 
            return false;
        if (x <= n) 
            return spf[x] == x;
        return alg::math::is_prime(x);
    }

    template <typename Tp>
    std::vector<std::pair<Tp, int>> factorize(Tp x) const {
        if (x <= n) {
            std::vector<std::pair<Tp, int>> fact;
            while (x > 1) {
                int p = spf[x];
                int cnt = 0;
                while (spf[x] == p) {
                    ++cnt;
                    x /= p;
                }
                fact.push_back({static_cast<Tp>(p), cnt});
            }
            return fact;
        } 
        return alg::math::factorize(x);
    }

    template <typename Tp>
    Tp phi(Tp x) const {
        auto fact = factorize(x);
        Tp res = x;
        for (auto [p, cnt] : fact) {
            res -= res / p;
        }
        return res;
    }

    template <typename Tp>
    Tp count_divisors(Tp x) const {
        auto fact = factorize(x);
        Tp cnt = 1;
        for (auto [p, e] : fact) {
            cnt *= (e + 1);
        }
        return cnt;
    }

    template <typename Res = void, typename Tp>
    auto sum_divisors(Tp x) const {
        using ret = std::conditional_t<std::is_void_v<Res>, Tp, Res>;
        auto fact = factorize(x);
        ret tot = 1;
        for (auto [p, cnt] : fact) {
            ret e = 1, sum = 1;
            for (int i = 1; i <= cnt; ++i) {
                e *= p;
                sum += e;
            }
            tot *= sum;
        }
        return tot;
    }

    template <typename Tp>
    std::vector<Tp> get_divisors(Tp x) const {
        auto fact = factorize(x);
        std::vector<Tp> divs = {1};
        for (auto [p, cnt] : fact) {
            int siz = divs.size();
            Tp e = 1;
            for (int i = 1; i <= cnt; ++i) {
                e *= p;
                for (int j = 0; j < siz; ++j) {
                    divs.push_back(divs[j] * e);
                }
            }
        }
        std::sort(divs.begin(), divs.end());
        return divs;
    }
    
    const std::vector<int>& get_primes() const {
        return primes;
    }

private:
    int n;
    std::vector<int> spf;
    std::vector<int> primes; 
};

} // namespace ds
} // namespace cp
#endif