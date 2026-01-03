/// @author Huy
/// @since  22:42:16 25/11/2025
/// @link   https://onlinejudge.org/index.php?option=com_onlinejudge&Itemid=8&page=show_problem&category=0&problem=3154&mosmsg=Submission+received+with+ID+30837664
#include "cp/init/lib.hpp"
#include "cp/ds/set/order_set.hpp"

constexpr int B = 600;

signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);

    int n, m, u;
    std::cin >> n >> m >> u;

    std::vector<int> a(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> a[i];
    }

    std::vector<cp::ds::multiset<int>> blocks(n / B + 1);
    for (int i = 0; i < n; ++i) {
        blocks[i / B].insert(a[i]);
    }

    auto range_query = [&](int l, int r, int v) -> int {
        int count = 0;
        int bl = l / B;
        int br = r / B;

        if (bl == br ) {
            for (int i = l; i <= r; ++i) {
                if (a[i] < v) {
                    ++count;
                }
            }
        } else {
            int end_first_block = (bl + 1) * B;
            for (int i = l; i < end_first_block && i < n; ++i) {
                if (a[i] < v) {
                    ++count;
                }
            }

            for (int b = bl + 1; b < br; ++b) {
                count += blocks[b].order_of_key(v);
            }

            int start_last_block = br * B;
            for (int i = start_last_block; i <= r; ++i) {
                if (a[i] < v) {
                    ++count;
                }
            }
        }

        return count;
    };

    auto modify = [&](int p, int val) -> void {
        int old_val = a[p];
        if (old_val == val) {
            return;
        }

        a[p] = val;
        int b = p / B;

        blocks[b].erase(blocks[b].lower_bound(old_val));
        blocks[b].insert(val);
    };

    for (int i = 0; i < m; ++i) {
        int l, r, v, p;
        std::cin >> l >> r >> v >> p;
        --l; --r; --p;

        int k = range_query(l, r, v);
        long long num = 1ll * u * k;
        long long den = 1ll * r - l + 1;
        int new_val = int(num / den);

        modify(p, new_val);
    }

    for (int i = 0; i < n; ++i) {
        std::cout << a[i] << '\n';
    }

    return 0;
}