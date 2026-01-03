#include <vector>

#ifndef CP_ALG_MATH_SIEVE_DIVISORS
#define CP_ALG_MATH_SIEVE_DIVISORS
namespace cp {
namespace alg {

std::pair<std::vector<int>,
          std::vector<int>> sieve_divisor(int n = 1000000) {
    std::vector<int> cnt(n + 1, 1), sum(n + 1, 1);

    cnt[0] = 0; sum[0] = 0;
    cnt[1] = 0; sum[1] = 0;
    for (int i = 2; i * i <= n; ++i) {
        ++cnt[i * i];
        sum[i * i] += i;
        for (int j = i + 1; i * j <= n; ++j) {
            cnt[i * j] += 2;
            sum[i * j] += i + j;
        }
    }

    return { cnt, sum };
}

} // namespace alg
} // namespace cp
#endif