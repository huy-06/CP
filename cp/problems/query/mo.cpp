/// @author Huy
/// @since  13:55:59 26/11/2025
/// @link   https://www.spoj.com/problems/KQUERY/ (TLE)
#include "cp/init/lib.hpp"
#include "cp/ds/set/order_set.hpp"

long long hibert_order(int x, int y, int pow, int rotate = 0) {
    if (pow == 0) {
        return 0;
    }        
    int h = 1 << (pow - 1);
    int seg = ((x < h) ? 0 : 1) * 2 + ((y < h) ? 0 : 1);      
    seg = (seg + rotate) & 3;
    static const int rotate_delta[4] = {3, 0, 0, 1};
    int nx = x & (h - 1);
    int ny = y & (h - 1);
    int nrot = (rotate + rotate_delta[seg]) & 3;
    long long sub_square_size = 1LL << (2 * pow - 2);
    long long res = seg * sub_square_size;
    long long add = hibert_order(nx, ny, pow - 1, nrot);
    res += (seg == 0 || seg == 3) ? add : (sub_square_size - add - 1);
    return res;
}

struct query {
    int l, r, k, i;
    long long ord;
};

signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);

    int n;
    std::cin >> n;

    std::vector<int> a(n);
    for  (int i = 0; i < n; ++i) {
        std::cin >> a[i];
    }

    int pow = 0;
    while ((1 << pow) < n) {
        ++pow;
    }

    int q;
    std::cin >> q;

    std::vector<query> queries(q);
    for (int i = 0; i < q; ++i) {
        int l, r, k;
        std::cin >> l >> r >> k;
        --l; --r;

        queries[i] = {l, r, k, i, hibert_order(l, r, pow, 0)};
    }
    std::sort(queries.begin(), queries.end(), [](const query& a, const query& b) {
        return a.ord < b.ord;
    });

    cp::ds::multiset<int> se;

    auto add = [&](int i) -> void {
        se.insert(a[i]);
    };

    auto remove = [&](int i) -> void {
        se.erase(se.find(a[i]));
    };

    std::vector<int> ans(q);
    int l = 0, r = -1;

    for (const query& i : queries) {
        while (l > i.l) add(--l);
        while (r < i.r) add(++r);
        while (l < i.l) remove(l++);
        while (r > i.r) remove(r--);
        //<
        debug(l, r);
        for (auto i : se) {
            std::cerr << i << ' ';
        }
        std::cerr << '\n';
        //>

        auto it = se.upper_bound(i.k);
        if (it == se.end()) {
            ans[i.i] = 0;
        } else {
            ans[i.i] = se.size() - se.order_of_key(*it);
        }
    }

    for (int i = 0; i < q; ++i) {
        std::cout << ans[i] << '\n';
    }

    return 0;
}