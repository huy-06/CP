#include <queue>
#include "../../../ds/edge/weight.hpp"
#include "../../../ds/graph/graph.hpp"
#include "../../utils/trace_path.hpp"

#ifndef CP_ALG_GRAPH_SHORTEST_PATH_DIJKSTRA
#define CP_ALG_GRAPH_SHORTEST_PATH_DIJKSTRA
namespace cp {
namespace alg {

template <typename Edge>
std::pair<std::vector<typename Edge::value_type>,
          std::vector<int>> dijkstra(const ds::graph<Edge>& g, int src) {
    using value_type = typename Edge::value_type;
    using edge_type  = Edge;

    int n = g.num_vertices();

    std::vector<int> par(n, -1);
    
    std::vector<value_type> dis(n, edge_type::inf);
    dis[src] = 0;
    
    using pii = std::pair<value_type, int>;
    std::priority_queue<pii, std::vector<pii>, std::greater<pii>> pq;
    pq.emplace(0, src);
    
    while (!pq.empty()) {
        value_type d;
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