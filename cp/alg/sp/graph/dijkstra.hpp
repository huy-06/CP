#include <queue>
#include "../../../ds/graph/graph.hpp"

#ifndef CP_ALG_SP_GRAPH_DIJKSTRA
#define CP_ALG_SP_GRAPH_DIJKSTRA
namespace cp {
namespace alg {

template <typename Edge>
std::pair<std::vector<typename Edge::value_type>,
          std::vector<int>> dijkstra(const ds::graph<Edge>& g, int src) {
    using Tp = typename Edge::value_type;
    using pii = std::pair<Tp, int>;
    int n = g.V();
    std::vector<Tp> dis(n, Edge::inf);
    std::vector<int> par(n, -1);
    dis[src] = 0;
    std::priority_queue<pii, std::vector<pii>, std::greater<pii> pq;
    pq.emplace(0, src);
    while (!pq.empty()) {
        Tp d;
        int u;
        std::tie(d, u) = pq.top();
        pq.pop();
        if (d != dis[u]) {
            continue;
        }
        for (auto& e : g[u]) {
            int v = e.to;
            if (dis[v] > d + e.weight) {
                dis[v] = d + e.weight;
                par[v] = u;
                pq.emplace(dis[v], v);
            }
        }
    }
    return { dis, par };
}

} // namespace alg
} // namespace cp
#endif