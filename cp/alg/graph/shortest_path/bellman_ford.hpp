
#include "../../../ds/edge/weight.hpp"
#include "../../../ds/graph/graph.hpp"
#include "../../utils/trace_path.hpp"

#ifndef CP_ALG_GRAPH_SHORTEST_PATH_BELLMAN_FORD
#define CP_ALG_GRAPH_SHORTEST_PATH_BELLMAN_FORD
namespace cp {
namespace alg {

template <typename Edge>
std::tuple<bool,
           std::vector<typename Edge::value_type>,
           std::vector<int>> bellman_ford(const ds::graph<Edge>& g, int src) {
    using value_type = typename Edge::value_type;
    using edge_type  = Edge;

    int n = g.num_vertices();

    std::vector<int> par(n, -1);

    std::vector<value_type> dis(n, Edge::inf);
    dis[src] = 0;

    std::vector<edge_type> edges = g.get_edges();

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