#include <queue>
#include "../../ds/edge/weight.hpp"
#include "../../ds/graph/graph.hpp"

#ifndef CP_ALG_GRAPH_SHORTEST_PATH_BOTTLENECK
#define CP_ALG_GRAPH_SHORTEST_PATH_BOTTLENECK
namespace cp { 
namespace alg {

/// @brief minimize maximum edge
template<typename Edge>
std::vector<typename Edge::value_type> bottleneck(const ds::graph<Edge>& g, int src = 0) {
    using value_type = Edge::value_type;
    using pii = std::pair<int, value_type>;

    int n = g.num_vertices();

    std::vector<value_type> dis(n, Edge::inf);
    dis[src] = 0;

    std::priority_queue<pii, std::vector<pii>, std::greater<pii>> pq;
    pq.emplace(Edge::inf, src);

    while (!pq.empty()) {
        value_type d;
        int u;
        std::tie(d, u) = pq.top();
        pq.pop();

        for (const auto& e : g[u]) {
            int v = e.to;
            value_type w  = e.weight;
            value_type nd = std::max(d, w);
            if (dis[v] > nd) {
                dis[v] = nd;
                pq.emplace(nd, v);
            }
        }
    }

    return dis;
}

} // namespace alg
} // namespace cp
#endif