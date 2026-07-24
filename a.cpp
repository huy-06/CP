/// @author Thuw
/// @since  07:48:37 24/07/2026
#include "cp/init/library.hpp"
#include "cp/ds/query/seg/persistent_segment_tree.hpp"

signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);

    int n, q;
    std::cin >> n >> q;

    std::vector<long long> a(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> a[i];
    }

    cp::ds::persistent_segment_tree<
        long long,
        [](long long a, long long b) -> long long {
            return a + b;
        },
        []() -> long long {
            return 0;
        }
    > seg(a);
    seg.reserve(q);

    debug(seg.get_root(0));

    while (q--) {
        int t;
        std::cin >> t;

        if (t == 1) {
            int k, a, x;
            std::cin >> k >> a >> x;
            --k; --a;

            seg.modify(k, a, x);
        } else if (t == 2) {
            int k, a, b;
            std::cin >> k >> a >> b;
            --k; --a;

            std::cout << seg.range_query(k, a, b) << '\n';;
        } else {
            int k;
            std::cin >> k;
            --k;


        }
    }

    return 0;
}
/*
[DEBUG]
[IN]
5 6
2 3 1 2 5
3 1
2 1 1 5
2 2 1 5
1 2 2 5
2 1 1 5
2 2 1 5
[OUT]
13
13
13
15
*/