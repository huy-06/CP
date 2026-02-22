// #if __cplusplus >= 202002L && !defined(USE_FFT)

#include <vector>
#include <algorithm>
#include "../../bit/bit_ceil.hpp"
#include "../modular/primitive_root.hpp"
#include "../../../ds/numeric/modular/static.hpp"

#ifndef CP_ALG_MATH_POLYNOMIAL_CONVOLUTION_NTT
#define CP_ALG_MATH_POLYNOMIAL_CONVOLUTION_NTT
namespace cp {

namespace internal {

template <typename mint, int g = alg::mod::primitive_root(mint::mod())>
class fft_info {
public:
    static constexpr int rank2 = __builtin_ctz(mint::mod() - 1);
    std::array<mint, rank2 + 1> root;   // root[i]^(2^i) == 1
    std::array<mint, rank2 + 1> iroot;  // root[i] * iroot[i] == 1

    std::array<mint, (rank2 - 2 + 1 > 0 ? rank2 - 2 + 1 : 0)> rate2;
    std::array<mint, (rank2 - 2 + 1 > 0 ? rank2 - 2 + 1 : 0)> irate2;

    std::array<mint, (rank2 - 3 + 1 > 0 ? rank2 - 3 + 1 : 0)> rate3;
    std::array<mint, (rank2 - 3 + 1 > 0 ? rank2 - 3 + 1 : 0)> irate3;
    
    fft_info() {
        root[rank2] = mint(g).pow((mint::mod() - 1) >> rank2);
        iroot[rank2] = root[rank2].inv();
        for (int i = rank2 - 1; i >= 0; i--) {
            root[i] = root[i + 1] * root[i + 1];
            iroot[i] = iroot[i + 1] * iroot[i + 1];
        }

        {
            mint prod = 1, iprod = 1;
            for (int i = 0; i <= rank2 - 2; i++) {
                rate2[i] = root[i + 2] * prod;
                irate2[i] = iroot[i + 2] * iprod;
                prod *= iroot[i + 2];
                iprod *= root[i + 2];
            }
        }
        {
            mint prod = 1, iprod = 1;
            for (int i = 0; i <= rank2 - 3; i++) {
                rate3[i] = root[i + 3] * prod;
                irate3[i] = iroot[i + 3] * iprod;
                prod *= iroot[i + 3];
                iprod *= root[i + 3];
            }
        }
    }
};

template <typename mint>
void butterfly(std::vector<mint>& a) {
    int n = static_cast<int>(a.size());
    int h = __builtin_ctz(static_cast<unsigned int>(n));

    static const fft_info<mint> info;

    int len = 0;  // a[i, i+(n>>len), i+2*(n>>len), ..] is transformed
    while (len < h) {
        if (h - len == 1) {
            int p = 1 << (h - len - 1);
            mint rot = 1;
            for (int s = 0; s < (1 << len); s++) {
                int offset = s << (h - len);
                for (int i = 0; i < p; i++) {
                    auto l = a[i + offset];
                    auto r = a[i + offset + p] * rot;
                    a[i + offset] = l + r;
                    a[i + offset + p] = l - r;
                }
                if (s + 1 != (1 << len))
                    rot *= info.rate2[__builtin_ctz(~static_cast<unsigned int>(s))];
            }
            len++;
        } else {
            // 4-base
            int p = 1 << (h - len - 2);
            mint rot = 1, imag = info.root[2];
            for (int s = 0; s < (1 << len); s++) {
                mint rot2 = rot * rot;
                mint rot3 = rot2 * rot;
                int offset = s << (h - len);
                for (int i = 0; i < p; i++) {
                    auto mod2 = 1ULL * mint::mod() * mint::mod();
                    auto a0 = 1ULL * a[i + offset].val();
                    auto a1 = 1ULL * a[i + offset + p].val() * rot.val();
                    auto a2 = 1ULL * a[i + offset + 2 * p].val() * rot2.val();
                    auto a3 = 1ULL * a[i + offset + 3 * p].val() * rot3.val();
                    auto a1na3imag =
                        1ULL * mint(a1 + mod2 - a3).val() * imag.val();
                    auto na2 = mod2 - a2;
                    a[i + offset] = a0 + a2 + a1 + a3;
                    a[i + offset + 1 * p] = a0 + a2 + (2 * mod2 - (a1 + a3));
                    a[i + offset + 2 * p] = a0 + na2 + a1na3imag;
                    a[i + offset + 3 * p] = a0 + na2 + (mod2 - a1na3imag);
                }
                if (s + 1 != (1 << len))
                    rot *= info.rate3[__builtin_ctz(~static_cast<unsigned int>(s))];
            }
            len += 2;
        }
    }
}

template <typename mint>
void butterfly_inv(std::vector<mint>& a) {
    int n = static_cast<int>(a.size());
    int h = __builtin_ctz(static_cast<unsigned int>(n));

    static const fft_info<mint> info;

    int len = h;  // a[i, i+(n>>len), i+2*(n>>len), ..] is transformed
    while (len) {
        if (len == 1) {
            int p = 1 << (h - len);
            mint irot = 1;
            for (int s = 0; s < (1 << (len - 1)); s++) {
                int offset = s << (h - len + 1);
                for (int i = 0; i < p; i++) {
                    auto l = a[i + offset];
                    auto r = a[i + offset + p];
                    a[i + offset] = l + r;
                    a[i + offset + p] =
                        static_cast<unsigned long long>(static_cast<unsigned int>(l.val() - r.val()) + mint::mod()) *
                        irot.val();
                    ;
                }
                if (s + 1 != (1 << (len - 1)))
                    irot *= info.irate2[__builtin_ctz(~static_cast<unsigned int>(s))];
            }
            len--;
        } else {
            // 4-base
            int p = 1 << (h - len);
            mint irot = 1, iimag = info.iroot[2];
            for (int s = 0; s < (1 << (len - 2)); s++) {
                mint irot2 = irot * irot;
                mint irot3 = irot2 * irot;
                int offset = s << (h - len + 2);
                for (int i = 0; i < p; i++) {
                    auto a0 = 1ULL * a[i + offset + 0 * p].val();
                    auto a1 = 1ULL * a[i + offset + 1 * p].val();
                    auto a2 = 1ULL * a[i + offset + 2 * p].val();
                    auto a3 = 1ULL * a[i + offset + 3 * p].val();

                    auto a2na3iimag =
                        1ULL *
                        mint((mint::mod() + a2 - a3) * iimag.val()).val();

                    a[i + offset] = a0 + a1 + a2 + a3;
                    a[i + offset + 1 * p] =
                        (a0 + (mint::mod() - a1) + a2na3iimag) * irot.val();
                    a[i + offset + 2 * p] =
                        (a0 + a1 + (mint::mod() - a2) + (mint::mod() - a3)) *
                        irot2.val();
                    a[i + offset + 3 * p] =
                        (a0 + (mint::mod() - a1) + (mint::mod() - a2na3iimag)) *
                        irot3.val();
                }
                if (s + 1 != (1 << (len - 2)))
                    irot *= info.irate3[__builtin_ctz(~static_cast<unsigned int>(s))];
            }
            len -= 2;
        }
    }
}

template <typename mint>
std::vector<mint> convolution_naive(const std::vector<mint>& a, 
                                    const std::vector<mint>& b) {
    int n = static_cast<int>(a.size());
    int m = static_cast<int>(b.size());
    std::vector<mint> ans(n + m - 1);
    if (n < m) {
        for (int j = 0; j < m; j++) {
            for (int i = 0; i < n; i++) {
                ans[i + j] += a[i] * b[j];
            }
        }
    } else {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                ans[i + j] += a[i] * b[j];
            }
        }
    }
    return ans;
}

template <typename mint>
std::vector<mint> convolution_fft(std::vector<mint> a, 
                                  std::vector<mint> b) {
    int n = static_cast<int>(a.size());
    int m = static_cast<int>(b.size());
    int z = static_cast<int>(alg::bit_ceil(static_cast<unsigned int>(n + m - 1)));
    a.resize(z);
    butterfly(a);
    b.resize(z);
    butterfly(b);
    for (int i = 0; i < z; i++) {
        a[i] *= b[i];
    }
    butterfly_inv(a);
    a.resize(n + m - 1);
    mint iz = mint(z).inv();
    for (int i = 0; i < n + m - 1; i++) a[i] *= iz;
    return a;
}

constexpr bool is_ntt_friendly(unsigned long long mod) {
    return (mod - 1) % (1ULL << 20) == 0;
}

} // namespace internal

namespace alg {

template <typename mint, typename std::enable_if<!std::is_integral<mint>::value && internal::is_ntt_friendly(mint::mod()), int>::type = 0>
std::vector<mint> convolution(std::vector<mint>&& a, 
                              std::vector<mint>&& b) {
    int n = static_cast<int>(a.size());
    int m = static_cast<int>(b.size());
    if (!n || !m) return {};

    int z = static_cast<int>(bit_ceil(static_cast<unsigned int>(n + m - 1)));
    assert((mint::mod() - 1) % z == 0);

    if (std::min(n, m) <= 60) {
        return internal::convolution_naive(std::move(a), std::move(b));
    }
    return internal::convolution_fft(std::move(a), std::move(b));
}

// cho mod int
template <typename mint, typename std::enable_if<!std::is_integral<mint>::value && internal::is_ntt_friendly(mint::mod()), int>::type = 0>
std::vector<mint> convolution(const std::vector<mint>& a, 
                              const std::vector<mint>& b) {
    int n = static_cast<int>(a.size());
    int m = static_cast<int>(b.size());
    if (!n || !m) return {};

    int z = static_cast<int>(bit_ceil(static_cast<unsigned int>(n + m - 1)));
    assert((mint::mod() - 1) % z == 0);

    if (std::min(n, m) <= 60) {
        return internal::convolution_naive(a, b);
    }
    return internal::convolution_fft(a, b);
}

// cho mod int
template <unsigned int mod = 998244353, typename Tp, typename std::enable_if<std::is_integral<Tp>::value, int>::type = 0>
std::vector<Tp> convolution(const std::vector<Tp>& a, 
                            const std::vector<Tp>& b) {
    int n = static_cast<int>(a.size());
    int m = static_cast<int>(b.size());
    if (!n || !m) return {};

    using mint = ds::static_mod_int<mod>;

    int z = static_cast<int>(bit_ceil(static_cast<unsigned int>(n + m - 1)));
    assert((mint::mod() - 1) % z == 0);

    std::vector<mint> a2(n), b2(m);
    for (int i = 0; i < n; i++) {
        a2[i] = mint(a[i]);
    }
    for (int i = 0; i < m; i++) {
        b2[i] = mint(b[i]);
    }
    auto c2 = convolution(std::move(a2), std::move(b2));
    std::vector<Tp> c(n + m - 1);
    for (int i = 0; i < n + m - 1; i++) {
        c[i] = c2[i].val();
    }
    return c;
}

template <typename mint, typename std::enable_if<!std::is_integral<mint>::value && !internal::is_ntt_friendly(mint::mod()), int>::type = 0>
std::vector<mint> convolution(const std::vector<mint>& a,
                              const std::vector<mint>& b) {
    int n = static_cast<int>(a.size());
    int m = static_cast<int>(b.size());
    if (!n || !m) return {};

    std::vector<long long> a_ll(n), b_ll(m);
    for (int i = 0; i < n; ++i) a_ll[i] = a[i].val();
    for (int i = 0; i < m; ++i) b_ll[i] = b[i].val();

    static constexpr unsigned long long MOD1 = 754974721;
    static constexpr unsigned long long MOD2 = 167772161;
    static constexpr unsigned long long MOD3 = 469762049;

    auto c1 = convolution<MOD1>(a_ll, b_ll);
    auto c2 = convolution<MOD2>(a_ll, b_ll);
    auto c3 = convolution<MOD3>(a_ll, b_ll);

    static constexpr unsigned long long inv1_2 =
        mod::inverse(MOD1, MOD2).second;
    static constexpr unsigned long long inv12_3 =
        mod::inverse(MOD1 * MOD2, MOD3).second;

    int siz = n + m - 1;
    std::vector<mint> res(siz);

    long long current_mod = mint::mod();
    long long m1 = MOD1 % current_mod;
    long long m12 = (m1 * (MOD2 % current_mod)) % current_mod;

    for (int i = 0; i < siz; ++i) {
        long long x1 = c1[i];

        long long x2 =  (static_cast<long long>(c2[i]) - x1) % static_cast<long long>(MOD2);
        if (x2 < 0) x2 += MOD2;
        x2 = (x2 * inv1_2) % MOD2;

        long long x3 = (static_cast<long long>(c3[i]) - (x1 + x2 * MOD1) % MOD3) % static_cast<long long>(MOD3);
        if (x3 < 0) x3 += MOD3;
        x3 = (x3 * inv12_3) % MOD3;

        mint val = x1;
        val += mint(x2) * m1;
        val += mint(x3) * m12;
        res[i] = val;
    }

    return res;
}

// cho long long
std::vector<long long> convolution(const std::vector<long long>& a, 
                                   const std::vector<long long>& b) {
    int n = static_cast<int>(a.size());
    int m = static_cast<int>(b.size());
    if (!n || !m) return {};

    static constexpr unsigned long long MOD1 = 754974721;  // 2^24
    static constexpr unsigned long long MOD2 = 167772161;  // 2^25
    static constexpr unsigned long long MOD3 = 469762049;  // 2^26
    static constexpr unsigned long long M2M3 = MOD2 * MOD3;
    static constexpr unsigned long long M1M3 = MOD1 * MOD3;
    static constexpr unsigned long long M1M2 = MOD1 * MOD2;
    static constexpr unsigned long long M1M2M3 = MOD1 * MOD2 * MOD3;

    static constexpr unsigned long long i1 =
        mod::inverse(MOD2 * MOD3, MOD1).second;
    static constexpr unsigned long long i2 =
        mod::inverse(MOD1 * MOD3, MOD2).second;
    static constexpr unsigned long long i3 =
        mod::inverse(MOD1 * MOD2, MOD3).second;
        
    static constexpr int MAX_AB_BIT = 24;
    static_assert(MOD1 % (1ull << MAX_AB_BIT) == 1, "MOD1 isn't enough to support an array length of 2^24.");
    static_assert(MOD2 % (1ull << MAX_AB_BIT) == 1, "MOD2 isn't enough to support an array length of 2^24.");
    static_assert(MOD3 % (1ull << MAX_AB_BIT) == 1, "MOD3 isn't enough to support an array length of 2^24.");
    assert(n + m - 1 <= (1 << MAX_AB_BIT));

    auto c1 = convolution<MOD1>(a, b);
    auto c2 = convolution<MOD2>(a, b);
    auto c3 = convolution<MOD3>(a, b);

    std::vector<long long> c(n + m - 1);
    for (int i = 0; i < n + m - 1; i++) {
        unsigned long long x = 0;
        x += (c1[i] * i1) % MOD1 * M2M3;
        x += (c2[i] * i2) % MOD2 * M1M3;
        x += (c3[i] * i3) % MOD3 * M1M2;
        // B = 2^63, -B <= x, r(real value) < B
        // (x, x - M, x - 2M, or x - 3M) = r (mod 2B)
        // r = c1[i] (mod MOD1)
        // focus on MOD1
        // r = x, x - M', x - 2M', x - 3M' (M' = M % 2^64) (mod 2B)
        // r = x,
        //     x - M' + (0 or 2B),
        //     x - 2M' + (0, 2B or 4B),
        //     x - 3M' + (0, 2B, 4B or 6B) (without mod!)
        // (r - x) = 0, (0)
        //           - M' + (0 or 2B), (1)
        //           -2M' + (0 or 2B or 4B), (2)
        //           -3M' + (0 or 2B or 4B or 6B) (3) (mod MOD1)
        // we checked that
        //   ((1) mod MOD1) mod 5 = 2
        //   ((2) mod MOD1) mod 5 = 3
        //   ((3) mod MOD1) mod 5 = 4
        long long diff =
            c1[i] - mod::safe_mod(static_cast<long long>(x), static_cast<long long>(MOD1));
        if (diff < 0) diff += MOD1;
        static constexpr unsigned long long offset[5] = {
            0, 0, M1M2M3, 2 * M1M2M3, 3 * M1M2M3};
        x -= offset[diff % 5];
        c[i] = x;
    }

    return c;
}

// cho int
std::vector<int> convolution(const std::vector<int>& a, 
                             const std::vector<int>& b) {
    auto ans = convolution(
        std::vector<long long>(a.begin(), a.end()),
        std::vector<long long>(b.begin(), b.end())
    );
    return std::vector<int>(ans.begin(), ans.end());
}

} // namespace alg

} // namespace cp
#endif

// #endif