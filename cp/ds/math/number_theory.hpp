#include <vector>
#include <algorithm>
#include <cmath>

#ifndef CP_DS_NUMBER_THEORY
#define CP_DS_NUMBER_THEORY
namespace cp {
namespace ds {

class number_theory {
public:
    number_theory(int sieve_limit = 1000000) : sieve_max(sieve_limit) {
        min_prime.assign(sieve_max + 1, 0);
        primes.reserve(sieve_limit / 10);
        
        min_prime[1] = 1;
        for (int i = 2; i <= sieve_max; ++i) {
            if (min_prime[i] == 0) {
                min_prime[i] = i;
                primes.push_back(i);
            }
            for (int p : primes) {
                if (p > min_prime[i] || (long long)i * p > sieve_max) break;
                min_prime[i * p] = p;
            }
        }

        primes.shrink_to_fit();
    }

    bool is_prime(long long n) {
        if (n <= sieve_max) return n > 1 && min_prime[n] == n;
        for (long long p : primes) {
            if (p * p > n) break;
            if (n % p == 0) return false;
        }
        return true;
    }

    std::vector<std::pair<long long, int>> factorize(long long n) {
        std::vector<std::pair<long long, int>> res;
        
        while (n > 1 && n <= sieve_max) {
            int p = min_prime[n];
            int count = 0;
            while (n % p == 0) {
                n /= p;
                count++;
            }
            res.push_back({1ll * p, count});
        }
        
        if (n > 1) {
            for (long long p : primes) {
                if (p * p > n) break;
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

    long long count_divisors(long long n) {
        if (n == 1) return 1;
        auto factors = factorize(n);
        long long ans = 1;
        for (auto& p : factors) ans *= (p.second + 1);
        return ans;
    }

    long long sum_divisors(long long n) {
        if (n == 1) return 1;
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
        if (n == 1) return {1};
        
        auto factors = factorize(n);
        
        long long num_divs = 1;
        for(auto& p : factors) num_divs *= (p.second + 1);
        
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

    long long phi(long long n) {
        if (n == 1) return 1;
        long long ans = n;
        while (n > 1) {
            int p = min_prime[n];
            while (n % p == 0) n /= p;
            ans -= ans / p;
        }
        return ans;
    }

private:
    int sieve_max;
    std::vector<int> min_prime;
    std::vector<int> primes;
};

} // namespace ds
} // namespace cp
#endif