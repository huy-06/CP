/// @author Huy
/// @since  08:21:21 23/12/2025
/// @link   https://www.codechef.com/problems/RNG?tab=statement
///         https://codeforces.com/blog/entry/111862
#include "cp/init/library.hpp"
#include "cp/alg/math/polynomial/convolution.hpp"

using mint = cp::ds::static_mod_int<104857601>;

mint bostan_mori(std::vector<mint> P, std::vector<mint> Q, long long n) {
    while (n > 0) {
        std::vector<mint> Q_neg = Q;
        for (int i = 1; i < Q_neg.size(); i += 2) {
            Q_neg[i] = -Q_neg[i];
        }

        std::vector<mint> U = cp::alg::convolution(P, Q_neg);
        std::vector<mint> V = cp::alg::convolution(Q, Q_neg);

        P.clear();
        for (int i = (n & 1); i < U.size(); i += 2) {
            P.push_back(U[i]);
        }

        Q.clear();
        for (int i = 0; i < V.size(); i += 2) {
            Q.push_back(V[i]);
        }

        n >>= 1;
    }

    return P[0];
}

signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);

    int k;
    long long n;
    std::cin >> k >> n;

    std::vector h(k, mint(0));
    for (int i = 0; i < k; ++i) {
        std::cin >> h[i];
    }

    std::vector a(k, 0ll);
    for (int i = 0; i < k; ++i) {
        std::cin >> a[i];
    }

    if (n <= k) {
        std::cout << mint(h[n - 1]) << '\n';
        return 0;
    }

    std::vector<mint> Q(k + 1);
    Q[0] = 1;
    for (int i = 1; i <= k; ++i) {
        Q[i] = -a[i - 1];
    }

    std::vector<mint> P = cp::alg::convolution(h, Q);
    P.resize(k);

    std::cout << bostan_mori(P, Q, n - 1) << '\n';

    return 0;
}