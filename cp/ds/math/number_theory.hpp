#include "../../alg/math/nt/factorize.hpp"

#ifndef CP_DS_NUMBER_THEORY
#define CP_DS_NUMBER_THEORY
namespace cp {
namespace ds {

class number_theory {
public:
    number_theory(int limit = 10000000) : n(limit) {
        spf.resize(n + 1);
        iota(spf.begin(), spf.end(), 0); 
        
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

    bool is_prime(long long x) {
        if (x <= n) 
            return x > 1 && spf[x] == x;
        return alg::is_prime(x);
    }

    std::vector<std::pair<long long, int>> factorize(long long x) {
        if (x <= n) {
            std::vector<std::pair<long long, int>> factors;
            while (x > 1) {
                int p = spf[x];
                int cnt = 0;
                while (spf[x] == p) {
                    ++cnt;
                    x /= p;
                }
                factors.push_back({p, cnt});
            }
            return factors;
        } 
        
        return alg::factorize(x);
    }

    long long phi(long long x) {
        auto factors = factorize(x);
        long long res = x;
        for (auto& f : factors) {
            res -= res / f.first;
        }
        return res;
    }

    long long count_divisors(long long x) {
        auto factors = factorize(x);
        long long cnt = 1;
        for (auto& f : factors) {
            cnt *= (f.second + 1);
        }
        return cnt;
    }

    long long sum_divisors(long long x) {
        auto factors = factorize(x);
        long long sum_total = 1;
        for (auto& f : factors) {
            long long p = f.first;
            long long sum = 1;
            long long p_pow = 1;
            for (int i = 1; i <= f.second; ++i) {
                p_pow *= p;
                sum += p_pow;
            }
            sum_total *= sum;
        }
        return sum_total;
    }

    std::vector<long long> get_divisors(long long x) {
        auto factors = factorize(x);
        std::vector<long long> divisors;
        
        std::function<void(int, long long)> dfs = [&](int idx, long long val) -> void {
            if (idx == factors.size()) {
                divisors.push_back(val);
                return;
            }
            long long p = factors[idx].first;
            long long p_pow = 1;
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