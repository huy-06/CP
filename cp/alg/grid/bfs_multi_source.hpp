#include <vector>
#include <algorithm>
#include <functional>
#include <queue>
#include "../../cst/direction.hpp"

#ifndef CP_ALG_SP_GRID_BFS_MULTI_SOURCE
#define CP_ALG_SP_GRID_BFS_MULTI_SOURCE
namespace cp {
namespace alg {

/// @brief Tìm đường đi đến điểm gần nhất.
template <
    typename Grid, 
    typename Tp = std::decay_t<decltype(std::declval<Grid>()[0][0])>,
    typename Dir = decltype(cst::dxy4)
>
auto bfs_multi_source(
    const Grid& grid,
    int xs, int ys,
    Tp target,
    std::function<bool(int, int)> passable,
    const Dir& dxy = cst::dxy4
) -> std::vector<std::pair<int, int>> {
    if (!passable(xs, ys)) {
        return {}; 
    }

    if (grid[xs][ys] == target) {
        return {{xs, ys}}; 
    }

    int n = static_cast<int>(grid.size());
    int m = static_cast<int>(grid[0].size());
    int tot = n * m;

    int is = xs * m + ys;
    std::vector<int> par(tot, -1);

    std::vector<char> vis(tot, 0);
    vis[is] = 1;

    std::queue<int> q;
    q.push(is);

    while (!q.empty()) {
        int cur = q.front();
        q.pop();

        int x = cur / m;
        int y = cur % m;

        for (const auto& it : dxy) {
            int dx, dy;
            std::tie(dx, dy) = it;

            int nx = x + dx;
            int ny = y + dy;
            
            if (nx < 0 || nx >= n || ny < 0 || ny >= m) {
                continue;
            }

            bool is_target = (grid[nx][ny] == target);
            if (!is_target && !passable(nx, ny)) {
                continue;
            }

            int nxt = nx * m + ny;
            if (!vis[nxt]) {
                vis[nxt] = 1;
                par[nxt] = cur;
                q.push(nxt);

                if (is_target) {
                    std::vector<std::pair<int, int>> path;
                    path.reserve(tot);

                    for (int i = nxt; i != -1; i = par[i]) {
                        path.emplace_back(i / m, i % m);
                        if (i == is) break;
                    }

                    path.shrink_to_fit();
                    std::reverse(path.begin(), path.end());

                    return path;
                }
            }
        }
    }

    return {};
}

} // namespace alg
} // namespace cp
#endif