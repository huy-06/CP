#include <queue>
#include "../../../ds/edge/weight.hpp"
#include "../../../ds/graph/graph.hpp"

#ifndef CP_ALG_GRAPH_SHORTEST_PATH_DIJKSTRA
#define CP_ALG_GRAPH_SHORTEST_PATH_DIJKSTRA

namespace cp {
namespace alg {

template <typename Edge>
std::pair<std::vector<typename Edge::value_type>, std::vector<int>> dijkstra(const ds::graph<Edge>& g, int src) {
    using value_type = typename Edge::value_type;
    int n = g.num_vertices();

    std::vector<int> par(n, -1);
    std::vector<value_type> dis(n, Edge::inf);
    dis[src] = 0;

    using pvi = std::pair<value_type, int>;
    std::priority_queue<pvi, std::vector<pvi>, std::greater<pvi>> pq;
    pq.emplace(0, src);

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (d > dis[u]) continue;

        for (const auto& e : g.adj(u)) {
            int v = e.to;
            value_type nd = d + e.weight;
            if (dis[v] > nd) {
                dis[v] = nd;
                par[v] = u;
                pq.emplace(nd, v);
            }
        }
    }

    return { dis, par };
}

} // namespace alg
} // namespace cp

#endif // CP_ALG_GRAPH_SHORTEST_PATH_DIJKSTRA