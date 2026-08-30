#include <vector>
#include "../mod/montgomery.hpp"
#include "../../../alg/math/mod/primitive_root.hpp"

#ifndef CP_DS_POLYNOMIAL
#define CP_DS_POLYNOMIAL
namespace cp {
namespace ds {

template <typename Tp>
class polynomial {
public:
    using value_type = Tp;

    polynomial(int n = 1, const value_type& v = value_type(0))
        : f(n, v) {}

    template <typename Up>
    polynomial(const std::vector<Up>& v)
        : f(v.begin(), v.end()) { trim(); }

    polynomial(std::vector<value_type>&& v)
        : f(std::move(v)) { trim(); }

    template <typename InputIt>
    polynomial(InputIt first, InputIt last)
        : f(first, last) { trim(); }

    polynomial(const std::initializer_list<value_type>& v)
        : f(v) { trim(); }

    int size() const { 
        return static_cast<int>(f.size()); 
    }
    
    bool empty() const {
        return f.empty();
    }
    
    int deg() const {
        return size() - 1;
    }
    
    std::vector<value_type> data() const {
        return f;
    }

    operator std::vector<value_type>() const {
        return f;
    }

    value_type& operator[](int i) {
        if (i >= size()) {
            f.resize(i + 1, value_type(0));
        }
        return f[i];
    }

    value_type operator[](int i) const {
        return (i >= 0 && i < size()) ? f[i] : value_type(0);
    }

    polynomial derivative() const {
        if (size() <= 1) return polynomial();

        polynomial res(size() - 1);
        for (int i = 1; i < size(); ++i) {
            res[i - 1] = f[i] * value_type(i);
        }
        return res;
    }

    polynomial integral() const {
        if (empty()) return polynomial();

        polynomial res(size() + 1);
        std::vector<value_type> mod_inv = pre_inv(size() + 1);

        for (int i = 0; i < size(); ++i) {
            res[i + 1] = f[i] * mod_inv[i + 1];
        }
        res.trim();

        return res;
    }

    polynomial inv(int n) const {
        assert(!empty());

        polynomial res({f[0].inv()});
        for (int m = 1; m < n; m *= 2) {
            polynomial cur(f.begin(), f.begin() + std::min(size(), m * 2));
            polynomial nxt = res * cur;

            nxt.f.resize(m * 2);
            for (int i = 0; i < (int) nxt.f.size(); ++i) {
                nxt.f[i] = -nxt.f[i];
            }

            nxt.f[0] += value_type(2);
            res *= nxt;
            res.f.resize(m * 2);
        }
        res.f.resize(n);

        return res;
    }

    polynomial log(int n) const {
        assert(!empty() && f[0] == value_type(1));
        polynomial res = (derivative() * inv(n)).integral();
        res.f.resize(n);
        return res;
    }

    // tính e^p
    polynomial exp(int n) const {
        assert(empty() || f[0] == value_type(0));

        polynomial res({value_type(1)});
        for (int m = 1; m < n; m *= 2) {
            polynomial cur(f.begin(), f.begin() + std::min(size(), m * 2));
            polynomial nxt = res.log(m * 2);

            for (int i = 0; i < (int) nxt.f.size(); ++i) {
                nxt.f[i] = -nxt.f[i];
            }

            nxt.f.resize(m * 2);
            nxt.f[0] += value_type(1);
            nxt += cur;
            res *= nxt;
            res.f.resize(m * 2);
        }
        res.f.resize(n);

        return res;
    }

    polynomial sqrt(int n) const {
        assert(!empty() && f[0] == value_type(1));

        polynomial res({value_type(1)});
        value_type inv2 = value_type(2).inv();

        for (int m = 1; m < n; m *= 2) {
            polynomial cur(f.begin(), f.begin() + std::min(size(), m * 2));
            res = (res + cur * res.inv(m * 2)) * polynomial({inv2});
            res.f.resize(m * 2);
        }
        res.f.resize(n);

        return res;
    }

    // Tính đa thức P^k lấy n phần tử đầu tiên
    polynomial pow(long long k, int n) const {
        if (empty() || n == 0) return polynomial();
        int sh = 0;
        while (sh < size() && f[sh] == value_type(0)) ++sh;
        if (sh == size() || sh * k >= n) return polynomial(n, value_type(0));
        
        value_type scale = f[sh];

        polynomial p = (*this >> sh) * polynomial({scale.inv()});
        p = p.log(n - sh * k);
        for (auto& val : p.f) {
            val *= value_type(k);
        }
        p = p.exp(n - sh * k) * polynomial({scale.pow(k)});

        return p << (sh * k);
    }

    // shift F(x+c): dịch biến nội hàm theo độ tối ưu FFT chuẩn. 
    polynomial shift(value_type c) const {
        if (empty()) return polynomial();
        int n = size();

        std::vector<value_type> fact = pre_fact(n);
        std::vector<value_type> ifact = pre_ifact(fact);

        polynomial a(n), b(n);
        value_type p = 1;

        for (int i = 0; i < n; ++i) {
            a.f[n - 1 - i] = f[i] * fact[i];
            b.f[i] = p * ifact[i];
            p *= c;
        }
        a *= b;

        polynomial res(n);
        for (int i = 0; i < n; ++i) {
            res.f[i] = a.f[n - 1 - i] * ifact[i];
        }
        return res;
    }

    value_type eval(const value_type& x) const {
        value_type res = 0;
        for (int i = size() - 1; i >= 0; --i) {
            res = res * x + f[i];
        }
        return res;
    }

    std::vector<value_type> multipoint_evaluation(const std::vector<value_type>& x) const {
        int m = x.size();
        if (m == 0) return {};
        
        std::vector<polynomial> tree(4 * m);
        
        auto build = [&](auto& self, int v, int l, int r) -> void {
            if (l == r) {
                tree[v] = polynomial({-x[l], value_type(1)});
                return;
            }
            int mid = l + (r - l) / 2;
            self(self, v * 2, l, mid);
            self(self, v * 2 + 1, mid + 1, r);
            tree[v] = tree[v * 2] * tree[v * 2 + 1];
        };
        build(build, 1, 0, m - 1);
        
        std::vector<value_type> res(m);
        auto eval = [&](auto& self, int v, int l, int r, const polynomial& p) -> void {
            polynomial cur = p % tree[v];
            if (l == r) {
                res[l] = cur.empty() ? value_type(0) : cur.f[0];
                return;
            }
            int mid = l + (r - l) / 2;
            self(self, v * 2, l, mid, cur);
            self(self, v * 2 + 1, mid + 1, r, cur);
        };
        eval(eval, 1, 0, m - 1, *this);
        return res;
    }

    static polynomial interpolation(const std::vector<value_type>& x, const std::vector<value_type>& y) {
        int m = x.size();
        assert(m == (int)y.size());
        if (m == 0) return polynomial();
        if (m == 1) return polynomial({y[0]});
        
        std::vector<polynomial> tree(4 * m);
        auto build = [&](auto& self, int v, int l, int r) -> void {
            if (l == r) {
                tree[v] = polynomial({-x[l], value_type(1)});
                return;
            }
            int mid = l + (r - l) / 2;
            self(self, v * 2, l, mid);
            self(self, v * 2 + 1, mid + 1, r);
            tree[v] = tree[v * 2] * tree[v * 2 + 1];
        };
        build(build, 1, 0, m - 1);
        
        polynomial m_der = tree[1].derivative();
        std::vector<value_type> weights = m_der.multipoint_evaluation(x);
        for (int i = 0; i < m; ++i) {
            weights[i] = y[i] * weights[i].inv();
        }
        
        auto solve = [&](auto& self, int v, int l, int r) -> polynomial {
            if (l == r) {
                return polynomial({weights[l]});
            }
            int mid = l + (r - l) / 2;
            polynomial l_poly = self(self, v * 2, l, mid);
            polynomial r_poly = self(self, v * 2 + 1, mid + 1, r);
            return l_poly * tree[v * 2 + 1] + r_poly * tree[v * 2];
        };
        
        return solve(solve, 1, 0, m - 1);
    }

    polynomial operator+() const {
        return *this;
    }

    polynomial operator-() const {
        polynomial res = *this;
        for (auto& val : res.f) {
            val = -val;
        }
        return res;
    }

    polynomial& operator+=(const polynomial& rhs) {
        if (size() < rhs.size()) f.resize(rhs.size());
        for (int i = 0; i < rhs.size(); ++i) {
            f[i] += rhs.f[i];
        }
        trim();
        return *this;
    }

    polynomial& operator-=(const polynomial& rhs) {
        if (size() < rhs.size()) f.resize(rhs.size());
        for (int i = 0; i < rhs.size(); ++i) {
            f[i] -= rhs.f[i];
        }
        trim();
        return *this;
    }

    polynomial& operator*=(const polynomial& rhs) {
        if (f.empty() || rhs.empty()) {
            f.clear();
        } else {
            f = conv(f, rhs.f);
            trim();
        }
        return *this;
    }

    polynomial& operator/=(const polynomial& rhs) {
        assert(!rhs.empty());
        if (size() < rhs.size()) {
            f.clear();
            return *this;
        }

        int k = size() - rhs.size() + 1;
        polynomial a(std::vector<value_type>(f.rbegin(), f.rbegin() + k));
        polynomial b(std::vector<value_type>(rhs.f.rbegin(), rhs.f.rend()));

        polynomial q = a * b.inv(k);
        q.f.resize(k, value_type(0));
        std::reverse(q.f.begin(), q.f.end());
        q.trim();

        f = std::move(q.f);
        return *this;
    }

    polynomial& operator%=(const polynomial& rhs) {
        if (size() < rhs.size()) return *this;
        polynomial q = *this;
        q /= rhs;
        *this -= q * rhs;
        f.resize(std::max(0, rhs.size() - 1));
        trim();
        return *this;
    }

    // Dịch phải toán học nhân hệ số
    polynomial operator<<(int k) const {
        if (k < 0) return *this >> (-k);
        if (empty()) return *this;

        std::vector<value_type> res(size() + k);
        for (int i = 0; i < size(); ++i) {
            res[i + k] = f[i];
        }
        return polynomial(res);
    }

    // Dịch trái nguyên thu gọn x 
    polynomial operator>>(int k) const {
        if (k < 0) return *this << (-k);
        if (k >= size()) return polynomial();
        return polynomial(std::vector<value_type>(f.begin() + k, f.end()));
    }

    friend polynomial operator+(polynomial lhs, const polynomial& rhs) {
        return lhs += rhs;
    }

    friend polynomial operator-(polynomial lhs, const polynomial& rhs) {
        return lhs -= rhs;
    }

    friend polynomial operator*(polynomial lhs, const polynomial& rhs) {
        return lhs *= rhs;
    }

    friend polynomial operator/(polynomial lhs, const polynomial& rhs) {
        return lhs /= rhs;
    }

    friend polynomial operator%(polynomial lhs, const polynomial& rhs) {
        return lhs %= rhs;
    }

    friend bool operator==(const polynomial& lhs, const polynomial& rhs) {
        return lhs.f == rhs.f;
    }

    friend bool operator!=(const polynomial& lhs, const polynomial& rhs) {
        return lhs.f != rhs.f;
    }

    friend std::istream& operator>>(std::istream& is, polynomial& p) {
        for (int i = 0; i < p.size(); ++i) is >> p.f[i];
        p.trim();
        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, const polynomial& p) {
        if (p.empty()) return os << '0';

        bool first = true;
        for (int i = p.size() - 1; i >= 0; --i) {
            if (p.f[i] == value_type(0)) {
                continue;
            }

            if (!first) os << " + ";
            os << p.f[i];
            if (i > 0) os << 'x' << (i > 1 ? "^" + std::to_string(i) : "");

            first = false;
        }
        if (first) os << '0';
        
        return os;
    }

private:
    std::vector<value_type> f;

    void trim() {
        while (!f.empty() && f.back() == value_type(0)) {
            f.pop_back();
        }
    }

    static std::vector<value_type> pre_inv(int n) {
        std::vector<value_type> v(n);
        if (n > 0) v[0] = 1;

        value_type p = 1;
        for (int i = 1; i < n; ++i) {
            v[i] = p;
            p *= i;
        }

        p = p.inv();
        for (int i = n - 1; i > 0; --i) {
            v[i] = v[i] * p;
            p *= i;
        }

        return v;
    }

    static std::vector<value_type> pre_fact(int n) {
        std::vector<value_type> v(n);

        value_type p = 1;
        for (int i = 0; i < n; ++i) {
            v[i] = p;
            p *= std::max(1, i + 1);
        }

        return v;
    }

    static std::vector<value_type> pre_ifact(const std::vector<value_type>& fact) {
        int n = fact.size();
        std::vector<value_type> v(n);
        if (n > 0) v[n - 1] = fact[n - 1].inv();
        for (int i = n - 1; i > 0; --i) {
            v[i - 1] = v[i] * value_type(i);
        }
        return v;
    }

    template <typename mint>
    struct ntt_info {
        mint rate2[30], rate3[30], root2;
        mint irate2[30], irate3[30], iroot2;

        ntt_info() {
            mint rt = mint(alg::mod::primitive_root(mint::mod()));
            int rank = __builtin_ctz(mint::mod() - 1);

            std::vector<mint> es(rank + 1), ies(rank + 1);
            es[rank] = rt.pow((mint::mod() - 1) >> rank);
            ies[rank] = es[rank].inv();

            for (int i = rank - 1; i >= 1; --i) {
                es[i] = es[i + 1] * es[i + 1]; 
                ies[i] = ies[i + 1] * ies[i + 1];
            }

            mint p = 1;
            for (int i = 0; i <= rank - 2; ++i) {
                rate2[i] = es[i + 2] * p;
                p *= ies[i + 2];
            }
            p = 1;
            for (int i = 0; i <= rank - 3; ++i) {
                rate3[i] = es[i + 3] * p;
                p *= ies[i + 3];
            }
            root2 = es[2];

            mint ip = 1;
            for (int i = 0; i <= rank - 2; ++i) {
                irate2[i] = ies[i + 2] * ip;
                ip *= es[i + 2];
            }
            ip = 1;
            for (int i = 0; i <= rank - 3; ++i) {
                irate3[i] = ies[i + 3] * ip;
                ip *= es[i + 3];
            }
            iroot2 = ies[2];
        }
    };

    template <typename mint>
    static void ntt(std::vector<mint>& a) {
        int n = int(a.size()), h = __builtin_ctz(n);
        if (n <= 1) return;

        static const ntt_info<mint> info;

        int len = 0;
        while (len < h) {
            if (h - len == 1) {
                int p = 1 << (h - len - 1);
                mint rot = 1;

                for (int s = 0; s < (1 << len); ++s) {
                    int offset = s << (h - len);

                    for (int i = 0; i < p; ++i) {
                        auto l = a[i + offset];
                        auto r = a[i + offset + p] * rot;

                        a[i + offset] = l + r;
                        a[i + offset + p] = l - r;
                    }

                    if (s + 1 != (1 << len))
                        rot *= info.rate2[__builtin_ctz(~(unsigned int)(s))];
                }

                ++len;
            } else {
                int p = 1 << (h - len - 2);
                mint rot = 1, imag = info.root2;

                for (int s = 0; s < (1 << len); ++s) {
                    mint rot2 = rot * rot;
                    mint rot3 = rot * rot2;
                    int offset = s << (h - len);

                    for (int i = 0; i < p; ++i) {
                        auto a0 = a[i + offset];
                        auto a1 = a[i + offset + p] * rot;
                        auto a2 = a[i + offset + 2 * p] * rot2;
                        auto a3 = a[i + offset + 3 * p] * rot3;

                        auto t0 = a0 + a2, t2 = a0 - a2;
                        auto t1 = a1 + a3, t3 = (a1 - a3) * imag;

                        a[i + offset]         = t0 + t1;
                        a[i + offset + p]     = t0 - t1;
                        a[i + offset + 2 * p] = t2 + t3;
                        a[i + offset + 3 * p] = t2 - t3;
                    }

                    if (s + 1 != (1 << len))
                        rot *= info.rate3[__builtin_ctz(~(unsigned int)(s))];
                }

                len += 2;
            }
        }
    }

    template <typename mint>
    static void intt(std::vector<mint>& a) {
        int n = int(a.size()), h = __builtin_ctz(n);
        if (n <= 1) return;

        static const ntt_info<mint> info;

        int len = h;
        while (len > 0) {
            if (len == 1) {
                int p = 1 << (h - len);
                mint rot = 1;

                for (int s = 0; s < (1 << (len - 1)); s++) {
                    int offset = s << (h - len + 1);

                    for (int i = 0; i < p; i++) {
                        auto l = a[i + offset];
                        auto r = a[i + offset + p];

                        a[i + offset] = l + r;
                        a[i + offset + p] = (l - r) * rot;
                    }

                    if (s + 1 != (1 << (len - 1)))
                        rot *= info.irate2[__builtin_ctz(~(unsigned int)(s))];
                }

                --len;
            } else {
                int p = 1 << (h - len);
                mint rot = 1, imag = info.iroot2;

                for (int s = 0; s < (1 << (len - 2)); s++) {
                    mint rot2 = rot * rot;
                    mint rot3 = rot * rot2;
                    int offset = s << (h - len + 2);

                    for (int i = 0; i < p; i++) {
                        auto a0 = a[i + offset];
                        auto a1 = a[i + offset + p];
                        auto a2 = a[i + offset + 2 * p];
                        auto a3 = a[i + offset + 3 * p];

                        auto t0 = a0 + a1, t2 = a0 - a1;
                        auto t1 = a2 + a3, t3 = (a2 - a3) * imag;

                        a[i + offset]         = t0 + t1;
                        a[i + offset + p]     = (t2 + t3) * rot;
                        a[i + offset + 2 * p] = (t0 - t1) * rot2;
                        a[i + offset + 3 * p] = (t2 - t3) * rot3;
                    }

                    if (s + 1 != (1 << (len - 2)))
                        rot *= info.irate3[__builtin_ctz(~(unsigned int)(s))];
                }
                
                len -= 2;
            }
        }

        mint invn = mint(n).inv();
        for (int i = 0; i < n; i++) a[i] *= invn;
    }

    template <typename Up>
    static std::vector<Up> brute_mul(const std::vector<Up>& a, const std::vector<Up>& b) {
        if (a.empty() || b.empty()) return {};

        std::vector<Up> res(a.size() + b.size() - 1, Up(0));
        for (int i = 0; i < (int)a.size(); ++i) {
            for (int j = 0; j < (int)b.size(); ++j) {
                res[i + j] += a[i] * b[j];
            }
        }
        return res;
    }

    static std::vector<value_type> conv(const std::vector<value_type>& a, const std::vector<value_type>& b) {
        int n = a.size(), m = b.size();
        if (std::min(n, m) <= 60) return brute_mul(a, b);

        int req = n + m - 1;
        int siz = std::bit_ceil(static_cast<unsigned int>(req));

        if constexpr (is_modint_v<value_type>) {
            if constexpr (value_type::mod() == 998244353 || value_type::mod() == 167772161 || value_type::mod() == 469762049 || value_type::mod() == 754974721 || std::countr_zero(value_type::mod() - 1) >= 15) {
                int rank = __builtin_ctz(value_type::mod() - 1);
                int len  = 1 << rank;

                if (req <= len) {
                    std::vector<value_type> f(siz), g(siz);
                    std::copy(a.begin(), a.end(), f.begin());
                    std::copy(b.begin(), b.end(), g.begin());

                    ntt(f); 
                    ntt(g);

                    for (int i = 0; i < siz; ++i) {
                        f[i] *= g[i];
                    }
                    intt(f); 
                    f.resize(req);

                    return f;
                }

                int h = len / 2;
                int b1 = (n + h - 1) / h;
                int b2 = (m + h - 1) / h;

                std::vector as(b1, std::vector(len, value_type(0)));
                std::vector bs(b2, std::vector(len, value_type(0)));

                for (int i = 0; i < b1; ++i) {
                    int l = i * h;
                    int r = std::min(n, l + h);

                    for (int k = l; k < r; ++k) {
                        as[i][k - l] = a[k];
                    }
                    ntt(as[i]);
                }
                for (int i = 0; i < b2; ++i) {
                    int l = i * h;
                    int r = std::min(m, l + h);

                    for (int k = l; k < r; ++k) {
                        bs[i][k - l] = b[k];
                    }
                    ntt(bs[i]);
                }

                int b3 = b1 + b2 - 1;
                std::vector cs(b3, std::vector(len, value_type(0)));
                for (int i = 0; i < b1; ++i) {
                    for (int j = 0; j < b2; ++j) {
                        auto& ai = as[i];
                        auto& bj = bs[j];
                        auto& cij = cs[i + j];

                        for (int k = 0; k < len; ++k) {
                            cij[k] += ai[k] * bj[k];
                        }
                    }
                }

                std::vector<value_type> res(req, value_type(0));
                for (int i = 0; i < b3; ++i) {
                    intt(cs[i]);
                    int offset = i * h;
                    int kmax = std::min(len, req - offset);

                    for (int k = 0; k < kmax; ++k) {
                        res[offset + k] += cs[i][k];
                    }
                }
                return res;
            }
        }

        if constexpr (is_modint_v<value_type> || std::is_integral_v<value_type>) {
            using m1 = montgomery_mod_int<998244353>;
            using m2 = montgomery_mod_int<754974721>;
            using m3 = montgomery_mod_int<469762049>;

            std::vector<m1> a1(siz), b1(siz);
            std::vector<m2> a2(siz), b2(siz);
            std::vector<m3> a3(siz), b3(siz);

            auto extract = [](const auto& x) -> unsigned long long {
                if constexpr (is_modint_v<value_type>) return x.val();
                else return x;
            };

            for (int i = 0; i < n; ++i) {
                auto v = extract(a[i]);
                a1[i] = v; 
                a2[i] = v; 
                a3[i] = v;
            }
            for (int i = 0; i < m; ++i) {
                auto v = extract(b[i]);
                b1[i] = v; 
                b2[i] = v; 
                b3[i] = v;
            }

            ntt(a1); ntt(b1);
            ntt(a2); ntt(b2);
            ntt(a3); ntt(b3);

            for (int i = 0; i < siz; ++i) {
                a1[i] *= b1[i]; a2[i] *= b2[i];
                a3[i] *= b3[i];
            }
            intt(a1);
            intt(a2);
            intt(a3);

            std::vector<value_type> res(req);

            constexpr unsigned long long p1 = 998244353;
            constexpr unsigned long long p2 = 754974721;

            m2 i1 = m2(p1).inv();
            m3 i2 = m3(p1 * p2).inv();

            for (int i = 0; i < req; ++i) {
                unsigned long long x = a1[i].val();
                unsigned long long y = ((a2[i] - static_cast<unsigned>(x % p2)) * i1).val();
                unsigned long long z = ((a3[i] - m3(x) - m3(y) * m3(p1)) * i2).val();
                res[i] = static_cast<value_type>(x + (__int128) y * p1 + (__int128) z * p1 * p2);
            }
            
            return res;
        } else {
            return brute_mul(a, b);
        }
    }
};

} // namespace ds
} // namespace cp
#endif