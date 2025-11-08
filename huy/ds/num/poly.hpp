
#include <vector>
#include "../../alg/math/mod/primitive_root.hpp"
#include "mod/static.hpp"

#ifndef HUY_DS_POLY
#define HUY_DS_POLY
namespace huy {
namespace ds {

template <typename Tp>
class poly {
public:    
    using value_type = Tp;

    constexpr poly(int n = 0) {
        if (n < 0) return;
        coef.resize(n, value_type(0));
    } 

    constexpr poly(std::initializer_list<value_type> il) : coef(il) {
        normalize();
    }   
    
    template <typename U>
    constexpr explicit poly(const std::vector<U>& c) {
        coef.resize(c.size());
        for (int i = 0; i < static_cast<int>(c.size()); ++i) {
            coef[i] = value_type(c[i]);
        }
        normalize();
    }

    template<std::input_iterator Iter>
    constexpr poly(Iter first, Iter last) : coef(first, last) {
        normalize();
    }

    template<typename Fun>
    constexpr poly(int n, Fun fun) {
        if (n < 0) return;
        coef.reserve(n);
        for (int i = 0; i < n; ++i) {
            coef.push_back(fun(i));
        }
        normalize();
    }

    constexpr int deg() const {
        return static_cast<int>(coef.size()) - 1;
    }

    constexpr bool is_zero() const {
        return coef.empty();
    }

    constexpr std::vector<value_type> coeff() const {
        return coef;
    }

    constexpr value_type& operator[](int i) {
        return coef[i];
    }

    constexpr const value_type& operator[](int i) const {
        return coef[i];
    }

    constexpr poly operator+() const {
        return *this;
    }

    constexpr poly operator-() const {
        poly res = *this;
        for (auto& x : res.coef) {
            x = -x;
        }
        return res;
    }

    constexpr poly& operator+=(const poly& rhs) {
        if (rhs.coef.size() > coef.size()) {
            coef.resize(rhs.coef.size());
        }
        for (int i = 0; i < static_cast<int>(rhs.coef.size()); ++i) {
            coef[i] += rhs.coef[i];
        }
        normalize();
        return *this;
    }

    constexpr poly& operator-=(const poly& rhs) {
        if (rhs.coef.size() > coef.size()) {
            coef.resize(rhs.coef.size());
        }
        for (int i = 0; i < static_cast<int>(rhs.coef.size()); ++i) {
            coef[i] -= rhs.coef[i];
        }
        normalize();
        return *this;
    }

    constexpr poly& operator*=(const poly& rhs) {
        if (coef.empty() || rhs.coef.empty()) {
            coef.clear();
            return *this;
        }
        if constexpr (is_static_mod_int<value_type>::value) {
            coef = mul(coef, rhs.coef);
        } else if constexpr (std::is_same<value_type, long long>::value) {
            coef = mul64(coef, rhs.coef);
        } else {
            coef = mul_navie(coef, rhs.coef);
        }
        normalize();
        return *this;
    }

    constexpr poly& operator/=(const poly& rhs) {
        if (rhs.is_zero()) throw std::runtime_error("Division by zero");
        if (deg() < rhs.deg()) {
            coef.clear();
            return *this;
        }
        int n = deg();
        int m = rhs.deg();
        poly q(n - m + 1);
        for (int i = n; i >= m; --i) {
            value_type coef = coef[i] / rhs.coef[m];
            q.coef[i - m] = coef;
            for (int j = 0; j <= m; ++j){
                coef[i - m + j] -= coef * rhs.coef[j];
            }
        }
        return q;
    }

    constexpr poly& operator%=(const poly& rhs) {
        if (rhs.is_zero()) throw std::runtime_error("Division by zero");
        if (deg() < rhs.deg()) return *this;
        int n = deg();
        int m = rhs.deg();
        for (int i = n; i >= m; --i) {
            value_type coef = coef[i] / rhs.coef[m];
            for (int j = 0; j <= m; ++j) {
                coef[i - m + j] -= coef * rhs.coef[j];
            }
        }
        normalize();
        return *this;
    }

    constexpr poly& operator*=(const value_type& rhs) noexcept {
        for (auto& x : coef) {
            x *= rhs;
        }
        return *this;
    }

    constexpr poly& operator/=(const value_type& rhs) noexcept {
        for (auto& x : coef) {
            x /= rhs;
        }
        return *this;
    }

    friend constexpr poly operator+(poly lhs, const poly& rhs) {
        return lhs += rhs;
    }

    friend constexpr poly operator-(poly lhs, const poly& rhs) {
        return lhs -= rhs;
    }

    friend poly operator*(poly lhs, const poly& rhs) {
        return lhs *= rhs;
    }

    friend constexpr poly operator/(poly lhs, const poly& rhs) {
        return lhs /= rhs;
    }

    friend constexpr poly operator%(poly lhs, const poly& rhs) {
        return lhs %= rhs;
    }

    friend std::istream& operator>>(std::istream& is, poly& p) {
        for (auto& x : p.coef) {
            is >> x;
        }
        p.normalize();
        return is;
    }
   
    friend std::ostream& operator<<(std::ostream& os, const poly& p) {
        bool first = true;
        int n = p.coef.size();
        for (int i = n - 1; i >= 0; --i) {
            value_type coef = p.coef[i];
            if (coef == 0) continue;
            if (!first) os << " + ";
            os << coef;
            if (i > 0) {
                os << 'x';
                if (i > 1) os << '^' << i;
            }
            first = false;
        }
        if (first) os << '0';
        return os;
    }

    constexpr poly shift(int k) const {
        if (k >= 0) {
            poly b = *this;
            b.coef.insert(b.coef.begin(), k, value_type(0));
            return b;
        } else if (this->deg() <= -k) {
            return poly();
        }
        return poly(coef.begin() + (-k), coef.end());
    }

    constexpr poly trunc(int k) const {
        poly f = *this;
        f.coef.resize(k);
        f.normalize();
        return f;
    }

    constexpr poly derivative() const {
        if (coef.empty()) return {};
        poly res(std::max(0, static_cast<int>(coef.size()) - 1));
        for (int i = 1; i < static_cast<int>(coef.size()); ++i) {
            res.coef[i - 1] = value_type(i) * coef[i];
        }
        return res;
    }

    constexpr poly integral() const {
        poly res(static_cast<int>(coef.size()) + 1);
        res.coef[0] = value_type(0);
        for (int i = 0; i < static_cast<int>(coef.size()); ++i) {
            res.coef[i + 1] = value_type(coef[i]) / value_type(i + 1);
        }
        return res;
    }

    constexpr poly inv(int m) const {
        poly res{coef[0].inv()};
        int k = 1;
        while (k < m) {
            k <<= 1;
            res = (res * (poly{2} - trunc(k)) * res).trunc(k);
        }
        return res.trunc(m);
    }

    constexpr poly log(int m) const {
        return (derivative() * inv(m)).integral().trunc(m);
    }

    constexpr poly exp(int m) const {
        poly res{1};
        int k = 1;
        while (k < m) {
            k <<= 1;
            res = (res * (poly{1} - res.log(k) + trunc(k))).trunc(k);
        }
        return res.trunc(m);
    }

    constexpr poly pow(int k, int m) const {
        int i = 0;
        while (i < static_cast<int>(coef.size()) && coef[i] == 0) {
            ++i;
        }
        if (i == static_cast<int>(coef.size()) || 1LL * i * k >= m) {
            return poly(m);
        }
        value_type v = coef[i];
        poly f = shift(-i) * v.inv();
        return (f.log(m - i * k) * k).exp(m - i * k).shift(i * k) * v.pow(k);
    }
    
    constexpr poly sqrt(int m) const {
        poly res{1};
        int k = 1;
        while (k < m) {
            k <<= 1;
            res = (res + (trunc(k) * res.inv(k)).trunc(k)) * value_type(2).inv();
        }
        return res.trunc(m);
    }


private:
    std::vector<value_type> coef;

    constexpr void normalize() {
        while (!coef.empty() && coef.back() == value_type(0)) {
            coef.pop_back();
        }
    }

    template <class mint, int g = alg::mod::primitive_root(mint::mod())>
    struct fft_info {
        static constexpr int rank2 = std::countr_zero(static_cast<unsigned int>(mint::mod() - 1));
        std::array<mint, rank2 + 1> root;
        std::array<mint, rank2 + 1> iroot;

        std::array<mint, std::max(0, rank2 - 2 + 1)> rate2;
        std::array<mint, std::max(0, rank2 - 2 + 1)> irate2;

        std::array<mint, std::max(0, rank2 - 3 + 1)> rate3;
        std::array<mint, std::max(0, rank2 - 3 + 1)> irate3;

        fft_info() {
            root[rank2] = mint(g).pow((mint::mod() - 1) >> rank2);
            iroot[rank2] = root[rank2].inv();
            for (int i = rank2 - 1; i >= 0; --i) {
                root[i] = root[i + 1] * root[i + 1];
                iroot[i] = iroot[i + 1] * iroot[i + 1];
            }
            {
                mint prod = 1, iprod = 1;
                for (int i = 0; i <= rank2 - 2; ++i) {
                    rate2[i] = root[i + 2] * prod;
                    irate2[i] = iroot[i + 2] * iprod;
                    prod *= iroot[i + 2];
                    iprod *= root[i + 2];
                }
            }
            {
                mint prod = 1, iprod = 1;
                for (int i = 0; i <= rank2 - 3; ++i) {
                    rate3[i] = root[i + 3] * prod;
                    irate3[i] = iroot[i + 3] * iprod;
                    prod *= iroot[i + 3];
                    iprod *= root[i + 3];
                }
            }
        }
    };

    template<class mint>
    constexpr void butterfly(std::vector<mint>& coef) {
        int n = static_cast<int>(coef.size());
        int h = std::countr_zero(static_cast<unsigned int>(n));

        static const fft_info<mint> info;

        int len = 0;
        while (len < h) {
            if (h - len == 1) {
                int p = 1 << (h - len - 1);
                mint rot = 1;
                for (int s = 0; s < (1 << len); ++s) {
                    int offset = s << (h - len);
                    for (int i = 0; i < p; ++i) {
                        auto l = coef[i + offset];
                        auto r = coef[i + offset + p] * rot;
                        coef[i + offset] = l + r;
                        coef[i + offset + p] = l - r;
                    }
                    if (s + 1 != (1 << len))
                        rot *= info.rate2[std::countr_zero(~static_cast<unsigned int>(s))];
                }
                len++;
            } else {
                int p = 1 << (h - len - 2);
                mint rot = 1, imag = info.root[2];
                for (int s = 0; s < (1 << len); ++s) {
                    mint rot2 = rot * rot;
                    mint rot3 = rot2 * rot;
                    int offset = s << (h - len);
                    for (int i = 0; i < p; ++i) {
                        auto mod2 = 1ULL * mint::mod() * mint::mod();
                        auto a0 = 1ULL * coef[i + offset].val();
                        auto a1 = 1ULL * coef[i + offset + p].val() * rot.val();
                        auto a2 = 1ULL * coef[i + offset + 2 * p].val() * rot2.val();
                        auto a3 = 1ULL * coef[i + offset + 3 * p].val() * rot3.val();
                        auto a1na3imag =
                            1ULL * mint(a1 + mod2 - a3).val() * imag.val();
                        auto na2 = mod2 - a2;
                        coef[i + offset] = a0 + a2 + a1 + a3;
                        coef[i + offset + 1 * p] = a0 + a2 + (2 * mod2 - (a1 + a3));
                        coef[i + offset + 2 * p] = a0 + na2 + a1na3imag;
                        coef[i + offset + 3 * p] = a0 + na2 + (mod2 - a1na3imag);
                    }
                    if (s + 1 != (1 << len))
                        rot *= info.rate3[std::countr_zero(~static_cast<unsigned int>(s))];
                }
                len += 2;
            }
        }
    }

    template<class mint>
    constexpr void butterfly_inv(std::vector<mint>& coef) {
        int n = static_cast<int>(coef.size());
        int h = std::countr_zero(static_cast<unsigned int>(n));

        static const fft_info<mint> info;

        int len = h;
        while (len) {
            if (len == 1) {
                int p = 1 << (h - len);
                mint irot = 1;
                for (int s = 0; s < (1 << (len - 1)); ++s) {
                    int offset = s << (h - len + 1);
                    for (int i = 0; i < p; ++i) {
                        auto l = coef[i + offset];
                        auto r = coef[i + offset + p];
                        coef[i + offset] = l + r;
                        coef[i + offset + p] =
                            static_cast<unsigned long long>(static_cast<unsigned int>(l.val() - r.val()) + mint::mod()) *
                            irot.val();
                        ;
                    }
                    if (s + 1 != (1 << (len - 1)))
                        irot *= info.irate2[std::countr_zero(~static_cast<unsigned int>(s))];
                }
                len--;
            } else {
                int p = 1 << (h - len);
                mint irot = 1, iimag = info.iroot[2];
                for (int s = 0; s < (1 << (len - 2)); ++s) {
                    mint irot2 = irot * irot;
                    mint irot3 = irot2 * irot;
                    int offset = s << (h - len + 2);
                    for (int i = 0; i < p; ++i) {
                        auto a0 = 1ULL * coef[i + offset + 0 * p].val();
                        auto a1 = 1ULL * coef[i + offset + 1 * p].val();
                        auto a2 = 1ULL * coef[i + offset + 2 * p].val();
                        auto a3 = 1ULL * coef[i + offset + 3 * p].val();

                        auto a2na3iimag =
                            1ULL *
                            mint((mint::mod() + a2 - a3) * iimag.val()).val();

                        coef[i + offset] = a0 + a1 + a2 + a3;
                        coef[i + offset + 1 * p] =
                            (a0 + (mint::mod() - a1) + a2na3iimag) * irot.val();
                        coef[i + offset + 2 * p] =
                            (a0 + a1 + (mint::mod() - a2) + (mint::mod() - a3)) *
                            irot2.val();
                        coef[i + offset + 3 * p] =
                            (a0 + (mint::mod() - a1) + (mint::mod() - a2na3iimag)) *
                            irot3.val();
                    }
                    if (s + 1 != (1 << (len - 2)))
                        irot *= info.irate3[std::countr_zero(~static_cast<unsigned int>(s))];
                }
                len -= 2;
            }
        }
    }

    template<typename Tp2>
    constexpr std::vector<Tp2> mul_navie(const std::vector<Tp2>& lhs, const std::vector<Tp2>& rhs) {
        int n = static_cast<int>(lhs.size());
        int m = static_cast<int>(rhs.size());
        std::vector<Tp2> res(n + m - 1);
        if (n < m) {
            for (int j = 0; j < m; ++j) {
                for (int i = 0; i < n; ++i) {
                    res[i + j] += lhs[i] * rhs[j];
                }
            }
        } else {
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < m; ++j) {
                    res[i + j] += lhs[i] * rhs[j];
                }
            }
        }
        return res;
    }

    template<class mint>
    constexpr std::vector<mint> mul_fft(std::vector<mint> lhs, std::vector<mint> rhs) {
        int n = static_cast<int>(lhs.size());
        int m = static_cast<int>(rhs.size());
        int z = std::bit_ceil(static_cast<unsigned int>(n + m - 1));
        lhs.resize(z);
        butterfly(lhs);
        rhs.resize(z);
        butterfly(rhs);
        for (int i = 0; i < z; ++i) {
            lhs[i] *= rhs[i];
        }
        butterfly_inv(lhs);
        lhs.resize(n + m - 1);
        mint iz = mint(z).inv();
        for (int i = 0; i < n + m - 1; ++i) {
            lhs[i] *= iz;
        }
        return lhs;
    }

    template<class mint>
    constexpr std::vector<mint> mul(const std::vector<mint>& lhs, const std::vector<mint>& rhs) {
        int n = static_cast<int>(lhs.size());
        int m = static_cast<int>(rhs.size());
        if (!n || !m) return {};

        int z = std::bit_ceil(static_cast<unsigned int>(n + m - 1));
        assert((mint::mod() - 1) % z == 0);

        if (std::min(n, m) <= 60){
            return mul_navie(lhs, rhs);
        }
        return mul_fft(lhs, rhs);
    }

    template<unsigned int MOD = 998244353>
    constexpr std::vector<value_type> mul_mod(const std::vector<value_type>& lhs, const std::vector<value_type>& rhs) {
        int n = static_cast<int>(lhs.size());
        int m = static_cast<int>(rhs.size());
        if (!n || !m) return {};

        using mint = static_mod_int<MOD>;

        int z = std::bit_ceil(static_cast<unsigned int>(n + m - 1));
        assert((mint::mod() - 1) % z == 0);

        std::vector<mint> a2(n), b2(m);
        for (int i = 0; i < n; ++i) {
            a2[i] = mint(lhs[i]);
        }
        for (int i = 0; i < m; ++i) {
            b2[i] = mint(rhs[i]);
        }
        std::vector<mint> c2 = mul(a2, b2);
        std::vector<value_type> c(n + m - 1);
        for (int i = 0; i < n + m - 1; ++i) {
            c[i] = value_type(c2[i].val());
        }
        return c;
    }

    constexpr std::vector<value_type> mul64(const std::vector<value_type>& lhs, const std::vector<value_type>& rhs) {
        int n = static_cast<int>(lhs.size());
        int m = static_cast<int>(rhs.size());
        if (!n || !m) return {};

        static constexpr unsigned long long MOD1 = 754974721;
        static constexpr unsigned long long MOD2 = 167772161;
        static constexpr unsigned long long MOD3 = 469762049;
        static constexpr unsigned long long M2M3 = MOD2 * MOD3;
        static constexpr unsigned long long M1M3 = MOD1 * MOD3;
        static constexpr unsigned long long M1M2 = MOD1 * MOD2;
        static constexpr unsigned long long M1M2M3 = MOD1 * MOD2 * MOD3;

        static constexpr unsigned long long i1 =
            alg::mod::inv_gcd(MOD2 * MOD3, MOD1).second;
        static constexpr unsigned long long i2 =
            alg::mod::inv_gcd(MOD1 * MOD3, MOD2).second;
        static constexpr unsigned long long i3 =
            alg::mod::inv_gcd(MOD1 * MOD2, MOD3).second;
            
        static constexpr int MAX_AB_BIT = 24;
        static_assert(MOD1 % (1ull << MAX_AB_BIT) == 1, "MOD1 isn't enough to support an array length of 2^24.");
        static_assert(MOD2 % (1ull << MAX_AB_BIT) == 1, "MOD2 isn't enough to support an array length of 2^24.");
        static_assert(MOD3 % (1ull << MAX_AB_BIT) == 1, "MOD3 isn't enough to support an array length of 2^24.");
        assert(n + m - 1 <= (1 << MAX_AB_BIT));

        auto c1 = mul_mod<MOD1>(lhs, rhs);
        auto c2 = mul_mod<MOD2>(lhs, rhs);
        auto c3 = mul_mod<MOD3>(lhs, rhs);

        std::vector<value_type> c(n + m - 1);
        for (int i = 0; i < n + m - 1; ++i) {
            unsigned long long x = 0;
            x += (c1[i] * i1) % MOD1 * M2M3;
            x += (c2[i] * i2) % MOD2 * M1M3;
            x += (c3[i] * i3) % MOD3 * M1M2;
            long long diff =
                c1[i] - ((x + MOD1) % MOD1);
            if (diff < 0) diff += MOD1;
            static constexpr unsigned long long offset[5] = {
                0, 0, M1M2M3, 2 * M1M2M3, 3 * M1M2M3
            };
            x -= offset[diff % 5];
            c[i] = x;
        }

        return c;
    }
};

} // namespace ds
} // namespace huy
#endif