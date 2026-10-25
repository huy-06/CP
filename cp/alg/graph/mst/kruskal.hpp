#include "../../../ds/edge/weight.hpp"
#include "../../../ds/tree/dsu/dsu.hpp"
#include "../../../ds/graph/graph.hpp"

#ifndef CP_ALG_GRAPH_MST
#define CP_ALG_GRAPH_MST

namespace cp {
namespace alg {

template <typename Edge>
std::pair<typename Edge::value_type, ds::graph<Edge>> kruskal(const ds::graph<Edge>& g) {
    using value_type = typename Edge::value_type;
    using edge_type  = Edge;

    int n = g.num_vertices();
    if (n <= 1) {
        return { 0, ds::graph<edge_type>(n, 0) };
    }

    std::vector<edge_type> edges = g.get_edges();
    std::sort(edges.begin(), edges.end(), [](const edge_type& a, const edge_type& b) {
        return a.weight < b.weight;
    });

    ds::graph<edge_type> mst(n, n - 1);
    ds::dsu d(n);

    value_type total = value_type{};
    int edge_count = 0;

    for (const auto& e : edges) {
        if (d.merge(e.from, e.to)) {
            total += e.weight;
            mst.add_unedge(e);
            ++edge_count;
            if (edge_count == n - 1) {
                break;
            }
        }
    }

    if (edge_count < n - 1) {
        return { edge_type::inf, ds::graph<edge_type>() };
    }

    mst.build();
    return { total, mst };
}

} // namespace alg
} // namespace cp

#endif // CP_ALG_GRAPH_MST