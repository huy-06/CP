
#include <queue>
#include "../../../ds/graph/graph.hpp"

#ifndef HUY_ALG_SP_GRAPH_BFS
#define HUY_ALG_SP_GRAPH_BFS
namespace huy {
namespace alg {

template <typename Edge>
std::pair<std::vector<int>,
          std::vector<int>> bfs(const ds::graph<Edge>& g, int src) {
    int n = g.V();
    std::vector<int> dis(n, -1);
    std::vector<int> par(n, -1);
    std::queue<int> q;
    q.push(src);
    dis[src] = 0;
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
} // namespace huy
#endif