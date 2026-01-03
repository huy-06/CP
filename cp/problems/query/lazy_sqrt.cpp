/// @author Huy
/// @since  21:33:25 26/11/2025
/// @link   https://codeforces.com/contest/121/problem/E
#include "cp/init/lib.hpp"

constexpr int V = 1e4;
constexpr int B = 320;

signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);

    std::vector<int> luckies;
    luckies.reserve(30);

    std::vector<char> is_lucky(V + 1);

    {
        std::queue<int> q;
        q.push(4);
        q.push(7);

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            luckies.push_back(u);
            is_lucky[u] = 1;

            for (auto d : {4, 7}) {
                int v = u * 10 + d;
                if (v <= V) {
                    q.push(v);
                }
            }
        }
    }

    int n, q;
    std::cin >> n >> q;
    
    int siz = n / B + 1;

    std::vector<std::vector<int>> blocks(siz, std::vector<int>(V + 1));

    std::vector<int> lazy(siz), a(n);

    for (int i = 0; i < n; ++i) {
        int v;
        std::cin >> v;

        a[i] = v;
        ++blocks[i / B][v];
    }

    while (q--) {
        std::string type;
        std::cin >>  type;

        if (type == "count") {
            int l, r;
            std::cin >> l >> r;
            --l; --r;

            int bl = l / B;
            int br = r / B;
            int ans = 0;

            if (bl == br) {
                for (int i = l; i <= r; ++i) {
                    if (is_lucky[a[i] + lazy[bl]]) {
                        ++ans;
                    }
                }
            } else {
                for (int i = l; i < (bl + 1) * B; ++i) {
                    if (is_lucky[a[i] + lazy[bl]]) {
                        ++ans;
                    }
                }

                for (int b = bl + 1; b < br; ++b) {
                    for (auto lucky : luckies) {
                        int need = lucky - lazy[b];
                        if (need >= 0 && need <= V) {
                            ans += blocks[b][need];
                        }
                    }
                }

                for (int i = br * B; i <= r; ++i) {
                    if (is_lucky[a[i] + lazy[br]]) {
                        ++ans;
                    }
                }
            }

            std::cout << ans << '\n';
        } else {
            int l, r, d;
            std::cin >> l >> r >> d;
            --l; --r;

            int bl = l / B;
            int br = r / B;

            if (bl == br) {
                for (int i = l; i <= r; ++i) {
                    --blocks[bl][a[i]];
                    a[i] += d;
                    ++blocks[bl][a[i]];
                }
            } else {
                for (int i = l; i < (bl + 1) * B; ++i) {
                    --blocks[bl][a[i]];
                    a[i] += d;
                    ++blocks[bl][a[i]];
                }

                for (int b = bl + 1; b < br; ++b) {
                    lazy[b] += d;
                }

                for (int i = br * B; i <= r; ++i) {
                    --blocks[br][a[i]];
                    a[i] += d;
                    ++blocks[br][a[i]];
                }
            }
        }
    }
   
    return 0;
}