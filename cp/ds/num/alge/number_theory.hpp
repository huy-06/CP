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
    bool is_prime(Tp x) {
        if (x <= 1) 
            return false;
        if (x <= n) 
            return spf[x] == x;
        return alg::is_prime(x);
    }

    template <typename Tp>
    std::vector<std::pair<Tp, int>> factorize(Tp x) {
        if (x <= n) {
            std::vector<std::pair<Tp, int>> factors;
            while (x > 1) {
                int p = spf[x];
                int cnt = 0;
                while (spf[x] == p) {
                    ++cnt;
                    x /= p;
                }
                factors.push_back({static_cast<Tp>(p), cnt});
            }
            return factors;
        } 
        
        return alg::factorize(x);
    }

    template <typename Tp>
    Tp phi(Tp x) {
        auto factors = factorize(x);
        Tp res = x;
        for (auto& f : factors) {
            res -= res / f.first;
        }
        return res;
    }

    template <typename Tp>
    Tp count_divisors(Tp x) {
        auto factors = factorize(x);
        Tp cnt = 1;
        for (auto& f : factors) {
            cnt *= (f.second + 1);
        }
        return cnt;
    }

    template <typename Tp>
    Tp sum_divisors(Tp x) {
        auto factors = factorize(x);
        Tp sum_total = 1;
        for (auto& f : factors) {
            Tp p = f.first;
            Tp sum = 1;
            Tp p_pow = 1;
            for (int i = 1; i <= f.second; ++i) {
                p_pow *= p;
                sum += p_pow;
            }
            sum_total *= sum;
        }
        return sum_total;
    }

    template <typename Tp>
    std::vector<Tp> get_divisors(Tp x) {
        auto factors = factorize(x);
        std::vector<Tp> divisors = {1};
        
        for (const auto& f : factors) {
            int sz = divisors.size();
            Tp p = f.first;
            Tp p_pow = 1;
            for (int i = 1; i <= f.second; ++i) {
                p_pow *= p;
                for (int j = 0; j < sz; ++j) {
                    divisors.push_back(divisors[j] * p_pow);
                }
            }
        }
    
        std::sort(divisors.begin(), divisors.end());
        return divisors;
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