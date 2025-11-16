
#include "../../../ds/graph/graph.hpp"

#ifndef CP_ALG_SP_GRAPH_BELLMAN_FORD
#define CP_ALG_SP_GRAPH_BELLMAN_FORD
namespace cp {
namespace alg {

template <typename Edge>
std::tuple<bool,
           std::vector<typename Edge::value_type>,
           std::vector<int>> bellman_ford(const ds::graph<Edge>& g, int src) {
    using Tp = typename Edge::value_type;
    int n = g.V();
    std::vector<Tp>  dis(n, Edge::inf);
    std::vector<int> par(n, -1);
    dis[src] = 0;
    auto edges = g.get_edges();
    for (int i = 0; i < n - 1; ++i) {
        bool upd = false;
        for (auto& e : edges) {
            if (dis[e.from] < Edge::inf && dis[e.to] > dis[e.from] + e.weight) {
                dis[e.to] = dis[e.from] + e.weight;
                par[e.to] = e.from;
                upd = true;
            }
        }
        if (!upd) break;
    }
    bool neg_cycle = false;
    for (auto& e : edges) {
        if (dis[e.from] < Edge::inf && dis[e.to] > dis[e.from] + e.weight) {
            par[e.to] = -2;
            neg_cycle = true;
        }
    }
    return { neg_cycle, dis, par };
}

} // namespace alg
} // namespace cp
#endif