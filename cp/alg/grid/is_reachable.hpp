#include <vector>
#include <algorithm>
#include <functional>
#include <queue>
#include <limits>
#include "../../cst/direction.hpp"

#ifndef CP_ALG_SP_GRID_IS_REACHABLE
#define CP_ALG_SP_GRID_IS_REACHABLE
namespace cp {
namespace alg {

/// @brief Kiểm tra có thể tới điểm đích hay không (a start algorithm).
template <
    typename Grid,
    typename Dir = decltype(cst::dxy4)
>
auto is_reachable(
    const Grid& grid,
    int xs, int ys,
    int xe, int ye,
    std::function<bool(int, int)> passable,
    const Dir& dxy = cst::dxy4,
    std::function<int(int, int)> h = nullptr
) -> bool {
    if (!passable(xs, ys) || !passable(xe, ye)) {
        return false;
    }
    
    if (xs == xe && ys == ye) {
        return true;
    }

    if (!h) {
        h = [xe, ye](int x, int y) {
            return std::abs(x - xe) + std::abs(y - ye);
        };
    }

    int n = static_cast<int>(grid.size());
    int m = static_cast<int>(grid[0].size());
    int tot = n * m;

    int is = xs * m + ys;
    int ie = xe * m + ye;

    std::vector<char> vis(tot, inf);
    dis[is] = 1;

    using pii = std::pair<int, int>;
    std::priority_queue<pii, std::vector<pii>, std::greater<pii>> pq;
    pq.push({h(xs, ys), is});

    while (!pq.empty()) {
        int cur = pq.top().second;
        pq.pop();

        if (cur == ie) {
            return true;
        }

        int x = cur / m;
        int y = cur % m;

        for (const auto& it : dxy) {
            int dx, dy;
            std::tie(dx, dy) = it;

            int nx = x + dx;
            int ny = y + dy;

            if (nx < 0 || nx >= n || ny < 0 || ny >= m || !passable(nx, ny)) {
                continue;
            }
            
            int nxt = nx * m + ny;            
            if (!vis[nxt]) {
                vis[nxt] = 1;
                pq.push({h(nx, ny), nxt});
            }
        }
    }

    return false;
}

} // namespace alg
} // namespace cp
#endif