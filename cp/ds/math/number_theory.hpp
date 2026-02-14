#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>

#ifndef CP_DS_NUMBER_THEORY_EXT
#define CP_DS_NUMBER_THEORY_EXT

namespace cp {
namespace ds {

template <int HARD_LIMIT = 500000000>
class number_theory {
public:
    number_theory(int initial_limit = 1000000) : sieve_max(0) {
        expand(std::min(initial_limit, HARD_LIMIT));
    }

    void expand(int n) {
        if (n > HARD_LIMIT) n = HARD_LIMIT;
        
        if (n <= sieve_max) return;

        min_prime.resize(n + 1, 0);
        
        for (int i = std::max(2, sieve_max + 1); i <= n; ++i) {
            if (min_prime[i] == 0) {
                min_prime[i] = i;
                primes.push_back(i);
            }
            for (int p : primes) {
                if (p > min_prime[i] || static_cast<long long>(i) * p > n) break;
                min_prime[i * p] = p;
            }
        }
        sieve_max = n;
    }

    bool is_prime(long long n) {
        if (n <= 1) 
            return false;

        if (n > sieve_max && n <= HARD_LIMIT) 
            expand(n);

        if (n <= sieve_max) 
            return min_prime[n] == n;

        for (long long p : primes) {
            if (p * p > n) 
                break;
            if (n % p == 0) 
                return false;
        }
        return true;
    }

    std::vector<std::pair<long long, int>> factorize(long long n) {
        std::vector<std::pair<long long, int>> res;
        
        if (n > sieve_max && n <= HARD_LIMIT) 
            expand(n);
        
        while (n > 1 && n <= sieve_max) {
            int p = min_prime[n];
            int count = 0;
            while (n % p == 0) {
                n /= p;
                count++;
            }
            res.push_back({static_cast<long long>(p), count});
        }
        
        if (n > 1) {
            for (long long p : primes) {
                if (p * p > n) 
                    break;
                if (n % p == 0) {
                    int count = 0;
                    while (n % p == 0) {
                        n /= p;
                        ++count;
                    }
                    res.push_back({p, count});
                }
            }
            if (n > 1) {
                res.push_back({n, 1});
            }
        }
        return res;
    }

    long long phi(long long n) {
        if (n == 1) 
            return 1;

        auto factors = factorize(n);

        long long ans = n;
        for (auto& p : factors) {
            ans -= ans / p.first;
        }
        return ans;
    }

    long long count_divisors(long long n) {
        if (n == 1) 
            return 1;

        auto factors = factorize(n);

        long long ans = 1;
        for (auto& p : factors) {
            ans *= (p.second + 1);
        }
        return ans;
    }

    long long sum_divisors(long long n) {
        if (n == 1) 
            return 1;

        auto factors = factorize(n);
        
        long long ans = 1;
        for (auto& p : factors) {
            long long p_val = p.first;
            int exponent = p.second;

            long long term = 1;
            long long p_pow = 1;
            for (int i = 0; i < exponent; ++i) {
                p_pow *= p_val;
                term += p_pow;
            }
            ans *= term;
        }
        return ans;
    }

    std::vector<long long> get_divisors(long long n) {
        if (n == 1) 
            return {1};

        auto factors = factorize(n);
        
        long long num_divs = 1;
        for(auto& p : factors) {
            num_divs *= (p.second + 1);
        }
        
        std::vector<long long> divs;
        divs.reserve(num_divs);
        divs.push_back(1);

        for (auto& p : factors) {
            long long prime = p.first;
            int exponent = p.second;
            int current_size = divs.size(); 
            
            for (int i = 0; i < current_size; ++i) {
                long long temp = divs[i];
                for (int e = 0; e < exponent; ++e) {
                    temp *= prime;
                    divs.push_back(temp);
                }
            }
        }
        std::sort(divs.begin(), divs.end());
        return divs;
    }

    int current_limit() const { 
        return sieve_max;
    }

private:
    int sieve_max;
    std::vector<int> min_prime;
    std::vector<int> primes;
};

} // namespace ds
} // namespace cp
#endif