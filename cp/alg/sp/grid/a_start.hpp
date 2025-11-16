#include <vector>
#include <algorithm>
#include <functional>
#include <queue>
#include <limits>

#ifndef CP_ALG_SP_GRID_A_START
#define CP_ALG_SP_GRID_A_START
namespace cp {
namespace alg {

template<typename Grid>
std::vector<std::pair<int, int>> a_star(const Grid& grid,
                                       int si, int sj,
                                       int ei, int ej,
                                       std::function<bool(int, int)> passable,
                                       const std::vector<std::pair<int, int>>& dirs = {{-1, 0}, {0, -1}, {0, 1}, {1, 0}},
                                       std::function<int(int, int)> h = nullptr ) {
    if (!passable(si, sj) || !passable(ei, ej)) {
        return {};
    }

    constexpr int inf = std::numeric_limits<int>::max() >> 1;
    int rows = static_cast<int>(grid.size());
    int cols = static_cast<int>(grid[0].size());
    int total = rows * cols;
    
    std::vector<int> cost(total, inf), par(total, -1);
    using pii = std::pair<int, int>;
    std::priority_queue<pii, std::vector<pii>, std::greater<pii>> open;

    int start = si * cols + sj;
    int end = ei * cols + ej;

    if (!h) {
        h = [ei, ej](int i, int j) {
            return std::abs(i - ei) + std::abs(j - ej);
        };
    }

    cost[start] = 0;
    open.push({h(si, sj), start});

    while (!open.empty()) {
        auto [f, cur] = open.top();
        open.pop();

        if (cur == end)
            break;

        int i = cur / cols, j = cur % cols;
        int ccost = cost[cur];
        if (f > ccost + h(i, j))
            continue;

        for (auto [di, dj] : dirs) {
            int ni = i + di, nj = j + dj;
            if (ni >= 0 && ni < rows && nj >= 0 && nj < cols && passable(ni, nj)) {
                int nxt = ni * cols + nj;
                int ncost = ccost + 1;
                if (ncost < cost[nxt]) {
                    cost[nxt] = ncost;
                    par[nxt] = cur;
                    open.push({ncost + h(ni, nj), nxt});
                }
            }
        }
    }

    std::vector<std::pair<int, int>> path;
    for (int cur = end; cur != -1; cur = par[cur]) {
        path.emplace_back(cur / cols, cur % cols);
        if (cur == start)
            break;
    }
    if (path.empty() || path.back().first != si || path.back().second != sj)
        return {};

    std::reverse(path.begin(), path.end());
    return path;
}

} // namespace alg
} // namespace cp
#endif