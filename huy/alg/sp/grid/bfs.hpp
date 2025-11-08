#include <vector>
#include <algorithm>
#include <functional>
#include <queue>

#ifndef HUY_ALG_SP_GRID_BFS
#define HUY_ALG_SP_GRID_BFS
namespace huy {
namespace alg {

template<typename Grid, typename Tp = std::decay_t<decltype(std::declval<Grid>()[0][0])>>
std::vector<std::pair<int, int>> breadthFirstSearch(
    const Grid& grid,
    int si, int sj,
    Tp target,
    std::function<bool(int, int)> passable,
    const std::vector<std::pair<int, int>>& dirs = {{-1, 0}, {0, -1}, {0, 1}, {1, 0}}
) {
    if (grid[si][sj] == target)
        return {{si, sj}};
    if (!passable(si, sj))
        return {};

    int rows = static_cast<int>(grid.size());
    int cols = static_cast<int>(grid[0].size());
    int total = rows * cols;
    std::vector<int> par(total, -1);
    std::vector<bool> vis(total, false);
    std::queue<std::pair<int, int>> q;

    q.push({si, sj});
    vis[si * cols + sj] = true;

    while (!q.empty()) {
        auto [i, j] = q.front();
        q.pop();

        for (const auto& [di, dj] : dirs) {
            int ni = i + di, nj = j + dj;
            int idx = ni * cols + nj;
            if (ni < 0 || ni >= rows || nj < 0 || nj >= cols)
                continue;

            bool is_target = (grid[nj][nj] == target);
            if (!is_target && !passable(ni, nj))
                continue;

            if (!vis[idx]) {
                vis[idx] = true;
                par[idx] = i * cols + j;
                q.push({ni, nj});
                if (is_target) {
                    std::vector<std::pair<int, int>> path;
                    for (int cur = idx; cur != -1; cur = par[cur]) {
                        path.emplace_back(cur / cols, cur % cols);
                        if (cur == si * cols + sj)
                            break;
                    }
                    std::reverse(path.begin(), path.end());
                    return path;
                }
            }
        }
    }
    return {};
}

} // namespace alg
} // namespace huy
#endif