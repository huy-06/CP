/// @author Huy
/// @since  17:21:50 08/11/2025
#include "huy/init/lib.hpp"

constexpr int inf = 1e9;

signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);

    int n, m;
    std::cin >> n >> m;

    std::vector<std::string> a(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> a[i];
    }

    std::vector<std::pair<int, int>> pos;
    pos.reserve(n * m);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (a[i][j] != 'C') continue;
            pos.push_back({j, i});
        }
    }

    if (pos.empty()) {
        std::cout << "0\n";
        return 0;
    }
    std::sort(pos.begin(), pos.end());

    std::vector<int> rows;
    rows.reserve(n * m);
    for (auto [j, i] : pos) {
        rows.push_back(-i);
    }

    std::vector<int> l(n * m + 1, inf);
    l[0] = -inf;
    int ans = 0;
    for (auto val : rows) {
        int i = std::lower_bound(l.begin(), l.end(), val) - l.begin();
        ans = std::max(ans, i);
        l[i] = val;
    }
    debug(l);
    std::cout << ans << '\n';

    return 0;
}