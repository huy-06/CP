/// @author Huy
/// @since  07:40:24 05/11/2025
/// @link   https://codeforces.com/gym/106161/problem/B  
#include "cp/init/lib.hpp"
#include "cp/cppx/lib.hpp"

signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);

    go (nint()) {
        ints(n, m, k, R);

        vi a(n), c(n);
        go (i, n) {
            a[i] = nint();
            c[i] = nint();
        }

        int S = 1ll << n;
        vvi M(S, vi(S, -inf));

        go (u, S) go (v, S) {
            int cost = 0, reward = 0;
            bool ok = true;
            go (i, n) {
                if ((v >> i) & 1) {
                    cost += ((u >> i) & 1) ? (c[i] + k) : c[i];
                    reward += a[i];
                    if (cost > m) {
                        ok = false;
                        break;
                    }
                }
            }
            if (ok && cost <= m) {
                M[u][v] = reward;
            }
        }

        // semiring max-plus
        fun (vvi, mul, const vvi& A, const vvi& B) {
            vvi C(S, vi(S, -inf));
            go (i, S) go (k, S) {
                if (A[i][k] <= -inf) continue;
                go (j, S) {
                    if (B[k][j] <= -inf) continue;
                    umax(C[i][j], A[i][k] + B[k][j]);
                }
            }
            return C;
        };

        vvi A(S, vi(S, -inf));
        go (i, S) A[i][i] = 0;

        while (R > 0) {
            if (R & 1) {
                A = mul(A, M);
            }
            M = mul(M, M);
            R >>= 1;
        }

        int ans = 0;
        go (j, S) {
            umax(ans, A[0][j]);
        }
        out(ans);
    }

    return 0;
}