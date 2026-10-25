#include "../../../ds/graph/graph.hpp"

#ifndef CP_ALG_GRAPH_SHORTEST_PATH_BFS
#define CP_ALG_GRAPH_SHORTEST_PATH_BFS

namespace cp {
namespace alg {

template <typename Edge>
std::pair<std::vector<int>, std::vector<int>> bfs(const ds::graph<Edge>& g, int src) {
    int n = g.num_vertices();

    std::vector<int> dis(n, -1), par(n, -1);
    dis[src] = 0;

    std::vector<int> q;
    q.reserve(n);
    q.push_back(src);

    for (size_t i = 0; i < q.size(); ++i) {
        int u = q[i];
        for (const auto& e : g.adj(u)) {
            int v = e.to;
            if (dis[v] == -1) {
                dis[v] = dis[u] + 1;
                par[v] = u;
                q.push_back(v);
            }
        }
    }

    return { dis, par };
}

} // namespace alg
} // namespace cp

#endif // CP_ALG_GRAPH_SHORTEST_PATH_BFS