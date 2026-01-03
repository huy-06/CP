#include <queue>
#include "../../../ds/edge/weight.hpp"
#include "../../../ds/tree/dsu.hpp"
#include "../../../ds/graph/graph.hpp"

#ifndef CP_ALG_GRAPH_MST
#define CP_ALG_GRAPH_MST
namespace cp { 
namespace alg {

/// @brief Tìm cây khung nhỏ nhất (minimum spanning tree).
/// @return Trả về tổng trọng số nhỏ nhất và đồ thị dạng cây khung của nó, nếu total == inf thì đồ thị không liên thông.
template <typename Edge>
std::pair<typename Edge::value_type,
          ds::graph<Edge>> kruskal(const ds::graph<Edge>& g) {

    using value_type = typename Edge::value_type;
    using edge_type  = Edge;

    int n = g.num_vertices();

    std::vector<edge_type> edges = g.get_edges();
    std::sort(edges.begin(), edges.end());

    ds::graph<edge_type> mst(n, n - 1);
    ds::dsu dsu(n);

    value_type total = value_type();
    int edge_count = 0;

    for (const auto& e : edges) {
        if (dsu.merge(e.from, e.to)) {
            total += e.weight;
            mst.add_unedge(e);

            ++edge_count;
            if (edge_count == n - 1) {
                break;
            }
        }
    }

    if (n > 0 && edge_count < n - 1) {
        return { edge_type::inf, ds::graph<edge_type>() };
    }

    return { total, mst };
}

} // namespace alg
} // namespace cp
#endif