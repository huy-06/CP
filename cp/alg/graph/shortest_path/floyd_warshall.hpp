#include "../../../ds/edge/weight.hpp"
#include "../../../ds/graph/graph.hpp"
#include "../../utils/trace_path.hpp"

#ifndef CP_ALG_GRAPH_SHORTEST_PATH_FLOY_WARSHALL
#define CP_ALG_GRAPH_SHORTEST_PATH_FLOY_WARSHALL
namespace cp {
namespace alg {

/// @brief Tìm đường đi ngắn nhất cho mọi đỉnh.
/// @return Trả về dis[][] và par[][]
template <typename Edge>
std::pair<std::vector<std::vector<typename Edge::value_type>>,
          std::vector<std::vector<int>>> floyd_warshall(const ds::graph<Edge>& g) {
    using value_type = typename Edge::value_type;
    using edge_type  = Edge;

    int n = g.num_vertices();

    std::vector<std::vector<value_type>> dis(n, std::vector<value_type>(n, edge_type::inf));
    std::vector<std::vector<int>> par(n, std::vector<int>(n, -1));

    for (int i = 0; i < n; ++i) {
        dis[i][i] = 0;
        par[i][i] = -1;
    }

    for (auto& e : g.get_edges()) {
        dis[e.from][e.to] = std::min(dis[e.from][e.to], e.weight);
        par[e.from][e.to] = e.from;
    }

    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            if (dis[i][k] >= edge_type::inf) {
                continue;
            }
            for (int j = 0; j < n; ++j) {
                if (dis[k][j] >= edge_type::inf) {
                    continue;
                }
                if (dis[i][j] > dis[i][k] + dis[k][j]) {
                    dis[i][j] = dis[i][k] + dis[k][j];
                    par[i][j] = par[k][j];
                }
            }
        }
    }

    return { dis, par };
}

} // namespace alg
} // namespace cp
#endif