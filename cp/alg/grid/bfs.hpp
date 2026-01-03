#include <vector>
#include <algorithm>
#include <functional>
#include <queue>
#include <limits>
#include "../../cst/direction.hpp"
#include "../../utils/trace_path.hpp"

#ifndef CP_ALG_SP_GRID_BFS
#define CP_ALG_SP_GRID_BFS
namespace cp {
namespace alg {


/// @brief  breadth first search trả về dis[][] và par[][]. Nếu dis[x][y] == -1 thì không có đường đi.
template <
    typename Grid,
    typename Dir = decltype(cst::dxy4)
>
auto bfs(
    const Grid& grid,
    int xs, int ys,
    std::function<bool(int, int)> passable,
    const Dir& dxy = cst::dxy4
) -> std::pair<std::vector<std::vector<int>>, 
               std::vector<std::vector<std::pair<int ,int>>>> {
    int n = static_cast<int>(grid.size());
    int m = static_cast<int>(grid[0].size());
    
    std::vector<std::vector<std::pair<int, int>>> par(n, std::vector<std::pair<int, int>>(m, {-1, -1}));

    std::vector<std::vector<int>> dis(n, std::vector<int>(m, -1));
    if (!passable(xs, ys)) {
        return {dis, par};
    }
    dis[xs][ys] = 0;

    std::queue<std::pair<int, int>> q;
    q.push({xs, ys});

    while (!q.empty()) {
        int x, y;
        std::tie(x, y) = q.front();
        q.pop();

        for (const auto& it : dxy) {
            int dx, dy;
            std::tie(dx, dy);

            int nx = x + dx;
            int ny = y + dy;

            if (nx < 0 || nx >= n || ny < 0 || ny >= m || !passable(nx, ny)) {
                continue;
            }

            dis[nx][ny] = dis[x][y] + 1;
            par[nx][ny] = {x, y};
            q.push({nx, ny});
        }
    }

    return {dis, par};
}

} // namespace alg
} // namespace cp
#endif