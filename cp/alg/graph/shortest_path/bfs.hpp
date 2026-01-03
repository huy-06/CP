
#include <queue>
#include "../../../ds/graph/graph.hpp"
#include "../../utils/trace_path.hpp"

#ifndef CP_ALG_GRAPH_SHORTEST_PATH_BFS
#define CP_ALG_GRAPH_SHORTEST_PATH_BFS
namespace cp {
namespace alg {

/// @brief  breadth first search trả về dis[] và par[]. Nếu dis[u] == -1 thì không có đường đi.
template <typename Edge>
std::pair<std::vector<int>,
          std::vector<int>> bfs(const ds::graph<Edge>& g, int src) {
    int n = g.num_vertices();

    std::vector<int> dis(n, -1), par(n, -1);
    dis[src] = 0;

    std::queue<int> q;
    q.push(src);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (const auto& e : g[u]) {
            int v = e.to;
            if (dis[v] == -1) {
                dis[v] = dis[u] + 1;
                par[v] = u;
                q.push(v);
            }
        }
    }
    
    return { dis, par };
}

} // namespace alg
} // namespace cp
#endif