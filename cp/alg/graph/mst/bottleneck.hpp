#include <queue>
#include "../../../ds/edge/weight.hpp"
#include "../../../ds/graph/graph.hpp"

#ifndef CP_ALG_GRAPH_SHORTEST_PATH_BOTTLENECK
#define CP_ALG_GRAPH_SHORTEST_PATH_BOTTLENECK

namespace cp { 
namespace alg {

template <typename Edge>
std::vector<typename Edge::value_type> bottleneck(const ds::graph<Edge>& g, int src = 0) {
    using value_type = typename Edge::value_type;
    int n = g.num_vertices();

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
            value_type nd = std::max(d, e.weight);
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

#endif // CP_ALG_GRAPH_SHORTEST_PATH_BOTTLENECK