#include <vector>
#include <complex>
#include <cmath>
#include <algorithm>
#include "../../../cst/real.hpp"
#include "../../../ds/numeric/modular/static.hpp"

#ifndef CP_ALG_MATH_POLYNOMIAL_CONVOLUTION_FFT
#define CP_ALG_MATH_POLYNOMIAL_CONVOLUTION_FFT
namespace cp {

namespace internal {

void fft(std::vector<std::complex<double>>& a, bool invert) {
    int n = static_cast<int>(a.size());
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) std::swap(a[i], a[j]);
    }
    for (int len = 2; len <= n; len <<= 1) {
        double ang = 2 * cst::pi / len * (invert ? -1 : 1);
        std::complex<double> wlen(std::cos(ang), std::sin(ang));
        for (int i = 0; i < n; i += len) {
            std::complex<double> w(1);
            for (int j = 0; j < len / 2; j++) {
                std::complex<double> u = a[i + j], v = a[i + j + len / 2] * w;
                a[i + j] = u + v;
                a[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }
    if (invert) {
        for (auto& x : a) x /= n;
    }
}

} // namespace internal

namespace alg {

// nhân đa thức cho long doublle
std::vector<long double> convolution(const std::vector<long double>& a, 
                                     const std::vector<long double>& b) {
    std::vector<long double> res;
    if (a.empty() || b.empty()) return res;
    int n = 1, sz = static_cast<int>(a.size() + b.size() - 1);
    while (n < sz) n <<= 1;
    
    std::vector<std::complex<double>> fa(a.begin(), a.end());
    std::vector<std::complex<double>> fb(b.begin(), b.end());
    fa.resize(n); fb.resize(n);
    
    internal::fft(fa, false);
    internal::fft(fb, false);
    for (int i = 0; i < n; i++) fa[i] *= fb[i];
    internal::fft(fa, true);
    
    res.resize(sz);
    for (int i = 0; i < sz; i++) res[i] = fa[i].real();
    return res;
}

#if __cplusplus < 202002L || defined(USE_FFT)

// nhân đa thức cho long long
std::vector<long long> convolution(const std::vector<long long>& a, 
                                   const std::vector<long long>& b) {
    std::vector<long long> res;
    if (a.empty() || b.empty()) return res;
    int n = 1, sz = static_cast<int>(a.size() + b.size() - 1);
    while (n < sz) n <<= 1;

    std::vector<std::complex<double>> fa(n), fb(n);
    for(size_t i = 0; i < a.size(); ++i) fa[i] = a[i];
    for(size_t i = 0; i < b.size(); ++i) fb[i] = b[i];

    internal::fft(fa, false);
    internal::fft(fb, false);
    for (int i = 0; i < n; i++) fa[i] *= fb[i];
    internal::fft(fa, true);

    res.resize(sz);
    for (int i = 0; i < sz; i++) {
        res[i] = static_cast<long long>(std::llround(fa[i].real()));
    }
    return res;
}

// nhân đa thức cho int
std::vector<int> convolution(const std::vector<int>& a, 
                             const std::vector<int>& b) {
    auto ans = convolution(
        std::vector<long long>(a.begin(), a.end()),
        std::vector<long long>(b.begin(), b.end())
    );
    return std::vector<int>(ans.begin(), ans.end());
}

// nhân đa thức cho mint
#if __cplusplus >= 202002L
template <auto mod>
#else
template <unsigned long long mod>
#endif
std::vector<ds::static_mod_int<mod>>
    convolution(
        const std::vector<ds::static_mod_int<mod>>& a, 
        const std::vector<ds::static_mod_int<mod>>& b) {

    using mint = ds::static_mod_int<mod>;

    std::vector<mint> res;
    if (a.empty() || b.empty()) return res;
    
    int n = 1, sz = static_cast<int>(a.size() + b.size() - 1);
    while (n < sz) n <<= 1;

    const int SPLIT_SHIFT = 15;
    const int SPLIT_MASK = (1 << SPLIT_SHIFT) - 1;

    std::vector<std::complex<double>> a0(n), a1(n), b0(n), b1(n);

    for(size_t i = 0; i < a.size(); ++i) {
        long long val = a[i].val();
        a0[i] = val & SPLIT_MASK;
        a1[i] = val >> SPLIT_SHIFT;
    }
    for(size_t i = 0; i < b.size(); ++i) {
        long long val = b[i].val();
        b0[i] = val & SPLIT_MASK;
        b1[i] = val >> SPLIT_SHIFT;
    }

    internal::fft(a0, false); internal::fft(a1, false);
    internal::fft(b0, false); internal::fft(b1, false);

    std::vector<std::complex<double>> c0(n), c1(n), c2(n);
    for(int i = 0; i < n; ++i) {
        c0[i] = a0[i] * b0[i];
        c1[i] = a1[i] * b0[i] + a0[i] * b1[i];
        c2[i] = a1[i] * b1[i];
    }

    internal::fft(c0, true);
    internal::fft(c1, true);
    internal::fft(c2, true);

    res.resize(sz);
    
    for(int i = 0; i < sz; ++i) {
        long long v0 = std::llround(c0[i].real()) % mod;
        long long v1 = std::llround(c1[i].real()) % mod;
        long long v2 = std::llround(c2[i].real()) % mod;

        mint term2 = mint(v2) * mint(1LL << (SPLIT_SHIFT * 2));
        mint term1 = mint(v1) * mint(1LL << SPLIT_SHIFT);
        mint term0 = mint(v0);
        
        res[i] = term2 + term1 + term0;
    }
    return res;
}

#endif

} // namespace alg

} // namespace cp
#endif