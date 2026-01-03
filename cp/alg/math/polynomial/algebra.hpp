#include "convolution.hpp"
#include "../../../ds/numeric/modular/combination.hpp"
#include "../number_theory/pow.hpp"
#include "../number_theory/sqrt.hpp"

#ifndef CP_ALG_MATH_POLYNOMIAL_ALGEBRA
#define CP_ALG_MATH_POLYNOMIAL_ALGEBRA
namespace cp {
namespace alg {

// Đạo hàm: P(x) -> P'(x)
template <typename Tp>
std::vector<Tp> derivative(const std::vector<Tp>& p) {
    if (p.size() <= 1) {
        return std::vector<Tp>();
    }
    std::vector<Tp> res(p.size() - 1);
    for (int i = 1; i < int(p.size()); ++i) {
        res[i - 1] = p[i] * Tp(i);
    }
    return res;
}

// Nguyên hàm: P(x) -> ∫P(x)dx (Hằng số C = 0)
#if __cplusplus >= 202002L
template <ds::modular mint>
std::vector<mint> integral(const std::vector<mint>& p) {
#else
template <unsigned long long mod>
std::vector<ds::static_mod_int<mod>> integral(const std::vector<ds::static_mod_int<mod>>& p) {
    using mint = ds::static_mod_int<mod>;
#endif
    if (p.empty()) {
        return std::vector<mint>();
    }
    static ds::combination<mint> comb;
    std::vector<mint> res(p.size() + 1);
    for (int i = 0; i < int(p.size()); ++i) {
        res[i + 1] = p[i] * comb.inv(i + 1);
    }
    return res;
}

// Nguyên hàm: P(x) -> ∫P(x)dx (Hằng số C = 0)
template <typename Tp>
std::vector<Tp> integral(const std::vector<Tp>& p) {
    if (p.empty()) {
        return std::vector<Tp>();
    }
    std::vector<Tp> res(p.size() + 1);
    for (int i = 0; i < int(p.size()); ++i) {
        res[i + 1] = p[i] / Tp(i + 1);
    }
    return res;
}

// Tính giá trị đa thức tại điểm x: P(x) (Horner's Method) - O(N)
template <typename Tp>
Tp evaluate(const std::vector<Tp>& p, const Tp& x) {
    if (p.empty()) {
        return Tp(0);
    }
    Tp res = 0;
    for (int i = p.size() - 1; i >= 0; --i) {
        res = res * x + p[i];
    }
    return res;
}

// Hàm nghịch đảo modulo x^n: P * P^-1 = 1 (mod x^n)
// Yêu cầu: p[0] != 0
template <typename Tp>
std::vector<Tp> inverse(const std::vector<Tp>& p, int n) {
    assert(!p.empty() && p[0] != Tp(0));
    std::vector<Tp> res = {Tp(1) / p[0]};

    for (int k = 1; k < n; k <<= 1) {
        std::vector<Tp> f(2 * k);
        for (int i = 0; i < 2 * k && i < int(p.size()); ++i) {
            f[i] = p[i];
        }

        f = convolution(f, res);
        f.resize(2 * k);

        for (auto& x : f) x = -x;
        f[0] += Tp(2);

        res = convolution(res, f);
        res.resize(2 * k);
    }

    res.resize(n);
    return res;
}

// Logarit tự nhiên: ln(P) mod x^n
// Công thức: Integ(P' * P^-1)
// Yêu cầu: P[0] == 1
template <typename Tp>
std::vector<Tp> log(const std::vector<Tp>& p, int n) {
    assert(!p.empty() && p[0] == Tp(1));
    std::vector<Tp> dp = derivative(p);
    std::vector<Tp> ip = inverse(p, n);
    dp = convolution(dp, ip);
    dp.resize(n - 1);
    return integral(dp);
}

// Hàm mũ: exp(P) mod x^n
// Newton Method: G_new = G_old * (1 - ln(G_old) + F)
// Yêu cầu: P[0] == 0
template <typename Tp>
std::vector<Tp> exp(const std::vector<Tp>& p, int n) {
    assert(p.empty() || p[0] == Tp(0));
    std::vector<Tp> res = {Tp(1)};

    for (int k = 1; k < n; k <<= 1) {
        std::vector<Tp> lg = log(res, 2 * k);

        std::vector<Tp> f(2 * k);
        for (int i = 0; i < 2 * k && i < int(p.size()); ++i) {
            f[i] = p[i];
        }

        for (int i = 0; i < 2 * k; ++i) {
            f[i] -= lg[i];
        }
        f[0] += Tp(1);

        res = convolution(res, f);
        res.resize(2 * k);
    }

    res.resize(n);
    return res;
}

// Lũy thừa đa thức: P^k mod x^n
// Công thức: P^k = exp(k * ln(P))
// Hỗ trợ k lớn (long long) và xử lý P[0] != 1 hoặc P có hệ số 0 ở đầu
template <typename Tp>
std::vector<Tp> pow(std::vector<Tp> p, long long k, int n) {
    if (k == 0) {
        std::vector<Tp> res(n, Tp(0));
        if (n > 0) res[0] = Tp(1);
        return res;
    }

    int shift = 0;
    while (shift < int(p.size()) && p[shift] == Tp(0)) {
        ++shift;
    }

    if (shift == int(p.size()) || shift * k >= n) {
        return std::vector<Tp>(n, Tp(0));
    }

    if (shift > 0) {
        p.erase(p.begin(), p.begin() + shift);
    }

    int m = n - shift * k;
    p.resize(m);

    Tp c = p[0];
    Tp inv_c = Tp(1) / c;
    for (auto& x : p) x *= inv_c;

    p = log(p, m);
    for (auto& x : p) x *= Tp(k);
    p = exp(p, m);

    Tp ck = alg::pow(c, k);
    for (auto& x : p) x *= ck;

    std::vector<Tp> res(n, Tp(0));
    int offset = shift * k;
    for (int i = 0; i < m; ++i) {
        res[i + offset] = p[i];
    }

    return res;
}

// Căn bậc hai đa thức: P(x) = Q(x)^2 (mod x^n)
template <typename Tp>
std::vector<Tp> sqrt(std::vector<Tp> p, int n) {
    if (p.empty()) {
        return std::vector<Tp>(n, Tp(0));
    }

    int zeros = 0;
    while (zeros < int(p.size()) && p[zeros] == Tp(0)) {
        ++zeros;
    }

    if (zeros == int(p.size())) {
        return std::vector<Tp>(n, Tp(0));
    }

    if (zeros % 2 != 0) {
        return {};
    }

    p.erase(p.begin(), p.begin() + zeros);
    int m = n - zeros / 2;

    Tp c = alg::sqrt(p[0]);
    Tp inv_2 = Tp(1) / Tp(2);

    std::vector<Tp> res = {c};
    for (int k = 1; k < m; k <<= 1) {
        int next_size = std::min(m, 2 * k);

        auto inv_res = inverse(res, next_size);

        std::vector<Tp> tmp(next_size);
        for (int i = 0; i < next_size && i < int(p.size()); ++i) {
            tmp[i] = p[i];
        }

        tmp = convolution(tmp, inv_res);
        tmp.resize(next_size);

        for (int i = 0; i < next_size; ++i) {
            if (i < int(res.size())) {
                tmp[i] += res[i];
            }
            tmp[i] *= inv_2;
        }
        res = tmp;
    }
    res.resize(m);

    std::vector<Tp> ans(zeros / 2, Tp(0));
    ans.insert(ans.end(), res.begin(), res.end());

    if (int(ans.size()) > n) {
        ans.resize(n);
    }
    return ans;
}

template <typename Tp>
std::vector<Tp> taylor_shift(const std::vector<Tp>& p, Tp c) {
    int n = int(p.size());
    if (n == 0) return {};
    if (c == Tp(0)) return p;

    static ds::combination<Tp> comb(n);
    
    std::vector<Tp> f(n), g(n);
    Tp cur_c = 1;
    for (int i = 0; i < n; ++i) {
        f[i] = p[i] * comb.fact(i);
        g[i] = cur_c * comb.inv_fact(i);
        cur_c *= c;
    }
    std::reverse(f.begin(), f.end());

    std::vector<Tp> res = convolution(f, g);
    res.resize(n);
    std::reverse(res.begin(), res.end());

    for (int i = 0; i < n; ++i) {
        res[i] *= comb.inv_fact(i);
    }
    return res;
}

} // namespace alg

namespace internal {

template <typename Tp>
std::pair<std::vector<Tp>, 
          std::vector<Tp>> div_mod(std::vector<Tp> a, std::vector<Tp> b) {
    while (!b.empty() && b.back() == Tp(0)) {
        b.pop_back();
    }

    if (b.empty()) {
        throw std::runtime_error("Division by zero!");
    }

    int n = int(a.size()) - 1;
    int m = int(b.size()) - 1;

    if (n < m) {
        return { {Tp(0)}, a };
    }

    std::vector<Tp> A = a, B = b;
    std::reverse(A.begin(), A.end());
    std::reverse(B.begin(), B.end());

    std::vector<Tp> B_trunc = B;
    B_trunc.resize(n - m + 1);

    std::vector<Tp> inv_B = alg::inverse(B_trunc, n - m + 1);


    std::vector<Tp> Q = alg::convolution(A, inv_B);
    Q.resize(n - m + 1);
    std::reverse(Q.begin(), Q.end());

    std::vector<Tp> prod = alg::convolution(b, Q);
    std::vector<Tp> R(m);
    for (int i = 0; i < m; ++i) {
        Tp val_prod = (i < int(prod.size())) ? prod[i] : Tp(0);
        Tp val_a = (i < int(a.size())) ? a[i] : Tp(0);
        R[i] = val_a - val_prod;
    }

    while (int(R.size()) > 1 && R.back() == Tp(0)) {
        R.pop_back();
    }

    return { Q, R };
}

template <typename Tp>
void build_subproduct_tree (
    int p, int l, int r,
    const std::vector<Tp>& pts,
    std::vector<std::vector<Tp>>& tree
) {
    if (l == r) {
        tree[p] = { -pts[l], Tp(1) };
        return;
    }
    int m = (l + r) / 2;
    build_subproduct_tree(2 * p, l, m, pts, tree);
    build_subproduct_tree(2 * p + 1, m + 1, r, pts, tree);
    tree[p] = alg::convolution(tree[2 * p], tree[2 * p + 1]);
}

template <typename Tp>
void eval_recursive (
    int p, int l, int r, 
    std::vector<Tp> poly,
    const std::vector<std::vector<Tp>>& tree,
    std::vector<Tp>& res
) {
    if (l == r) {
        if (!poly.empty()) {
            res[l] = poly[0];
        } else {
            res[l] = Tp(0);
        }
        return;
    }

    int m = (l + r) / 2;

    auto left = div_mod(poly, tree[2 * p]);
    eval_recursive(2 * p, l, m, left.second, tree, res);

    auto right = div_mod(poly, tree[2 * p + 1]);
    eval_recursive(2 * p + 1, m + 1, r, right.second, tree, res);
}

template <typename Tp>
std::vector<Tp> interp_recursive (
    int p, int l, int r,
    const std::vector<Tp>& y,
    const std::vector<std::vector<Tp>>& tree
) {
    if (l == r) {
        return { y[l] };
    }

    int m = (l + r) / 2;

    auto left = interp_recursive(2 * p, l, m, y, tree);
    auto right = interp_recursive(2 * p + 1, m + 1, r, y, tree);

    auto p1 = alg::convolution(left, tree[2 * p + 1]);
    auto p2 = alg::convolution(right, tree[2 * p]);

    int siz = std::max(p1.size(), p2.size());
    p1.resize(siz); p2.resize(siz);

    for (int i = 0; i < siz; ++i) {
        p1[i] += p2[i];
    }
    return p1;
}

} // namespace internal

namespace alg {

template <typename Tp>
std::vector<Tp> multipoint_evaluation(const std::vector<Tp>& p, const std::vector<Tp>& pts) {
    if (pts.empty()) return {};
    if (p.empty()) return std::vector<Tp>(pts.size(), Tp(0));

    int m = int(pts.size());
    std::vector<std::vector<Tp>> tree(4 * m);
    
    internal::build_subproduct_tree(1, 0, m - 1, pts, tree);

    std::vector<Tp> res(m);

    auto root = internal::div_mod(p, tree[1]);
    internal::eval_recursive(1, 0, m - 1, root.second, tree, res);

    return res;
}

template <typename Tp>
std::vector<Tp> interpolation(const std::vector<Tp>& x, const std::vector<Tp>& y) {
    assert(x.size() == y.size());
    int n = int(x.size());
    if (n == 0) return {};

    std::vector<std::vector<Tp>> tree(4 * n);
    internal::build_subproduct_tree(1, 0, n - 1, x, tree);

    std::vector<Tp> M = tree[1];
    std::vector<Tp> dM = derivative(M);

    std::vector<Tp> vals = multipoint_evaluation(dM, x);

    std::vector<Tp> y_div_dM(n);
    for (int i = 0; i < n; ++i) {
        y_div_dM[i] = y[i] / vals[i];
    }

    return internal::interp_recursive(1, 0, n - 1, y_div_dM, tree);
}

} // namespace alg

} // namespace cp
#endif