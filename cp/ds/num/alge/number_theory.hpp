#include "../../../alg/math/nt/factorize.hpp"

#ifndef CP_DS_NUMBER_THEORY
#define CP_DS_NUMBER_THEORY

namespace cp {
namespace ds {

template <int N = 1'000'000>
class number_theory {
    static_assert(N >= 0, "N must be non-negative");

public:
    template <typename Tp>
    static constexpr bool is_prime(Tp x) {
        if (x <= 1) {
            return false;
        }
        if (in_sieve(x)) {
            return get_table().first[x] == static_cast<int>(x);
        }
        return alg::math::is_prime(x);
    }

    template <typename Tp>
    static constexpr std::vector<std::pair<Tp, int>> factorize(Tp x) {
        if (x <= 1) {
            return {};
        }
        if (!in_sieve(x)) {
            return alg::math::factorize(x);
        }

        const auto& spf = get_table().first;

        std::vector<std::pair<Tp, int>> facts;
        facts.reserve(8);

        for (int n = static_cast<int>(x); n > 1;) {
            const int p = spf[n];
            int e = 0;
            do {
                n /= p;
                ++e;
            } while (spf[n] == p);
            facts.emplace_back(p, e);
        }

        return facts;
    }

    template <typename Tp>
    static constexpr Tp phi(Tp x) {
        if (x <= 0) {
            return 0;
        }

        Tp res = x;
        for (const auto& [p, e] : factorize(x)) {
            res -= res / p;
        }
        return res;
    }

    template <typename Tp>
    static constexpr Tp count_divisors(Tp x) {
        if (x <= 0) {
            return 0;
        }

        Tp cnt = 1;
        for (const auto& [p, e] : factorize(x)) {
            cnt *= e + 1;
        }
        return cnt;
    }

    template <typename Ret = void, typename Tp>
    static constexpr auto sum_divisors(Tp x) {
        using ret_t = std::conditional_t<std::is_void_v<Ret>, Tp, Ret>;

        ret_t tot = x > 0 ? 1 : 0;
        for (auto [p, e] : factorize(x)) {
            ret_t s = 1, t = 1;
            while (e--) {
                s += t *= static_cast<ret_t>(p);
            }
            tot *= s;
        }
        return tot;
    }

    template <typename Tp>
    static constexpr std::vector<Tp> get_divisors(Tp x) {
        if (x <= 0) {
            return {};
        }

        const auto facts = factorize(x);

        std::size_t tot = 1;
        for (const auto& [p, e] : facts) {
            tot *= e + 1;
        }

        std::vector<Tp> divs;
        divs.reserve(tot);
        divs.push_back(1);

        for (const auto& [p, e] : facts) {
            const std::size_t siz = divs.size();

            Tp t = 1;
            for (int i = 0; i < e; ++i) {
                t *= p;
                for (std::size_t j = 0; j < siz; ++j) {
                    divs.push_back(divs[j] * t);
                }
            }
        }
        std::sort(divs.begin(), divs.end());

        return divs;
    }

    static const std::vector<int>& get_primes() {
        return get_table().second;
    }

    template <typename Tp>
    static std::span<const int> get_primes(Tp x) {
        if (x < 2) {
            return {};
        }

        const auto& primes = get_table().second;
        return std::span<const int>(primes.begin(), std::ranges::upper_bound(primes, x));
    }

private:
    static const std::pair<std::vector<int>, std::vector<int>>& get_table() { // spf, primes
        static const std::pair<std::vector<int>, std::vector<int>> tbl = [] {
            std::pair<std::vector<int>, std::vector<int>> t;

            auto& [spf, primes] = t;
            spf.assign(N + 1, 0);

            if constexpr (N >= 16) {
                primes.reserve(static_cast<std::size_t>(N / (std::log(N) - 1.1)));
            }

            for (int i = 2; i <= N; ++i) {
                if (!spf[i]) {
                    spf[i] = i;
                    primes.push_back(i);
                }

                const int lp = spf[i];
                for (int p : primes) {
                    if (p > lp || 1LL * i * p > N) {
                        break;
                    }
                    spf[i * p] = p;
                }
            }

            return t;
        }();

        return tbl;
    }

    template <typename Tp>
    static constexpr bool in_sieve(Tp x) {
        using com_t = std::common_type_t<Tp, int>;
        return !std::is_constant_evaluated() && static_cast<com_t>(x) <= static_cast<com_t>(N);
    }
};

} // namespace cp
} // namespace ds

#endif