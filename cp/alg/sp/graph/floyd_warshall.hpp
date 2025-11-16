#include "../../../ds/graph/graph.hpp"

#ifndef CP_ALG_SP_GRAPH_FLOY_WARSHALL
#define CP_ALG_SP_GRAPH_FLOY_WARSHALL
namespace cp {
namespace alg {

template <typename Edge>
std::pair<std::vector<std::vector<typename Edge::value_type>>,
          std::vector<std::vector<int>>> floyd_warshall(const ds::graph<Edge>& g) {
    using Tp = typename Edge::value_type;
    int n = g.V();
    std::vector<std::vector<Tp>>  dis(n, std::vector<Tp>(n, Edge::inf));
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
            if (dis[i][k] >= Edge::inf) {
                continue;
            }
            for (int j = 0; j < n; ++j) {
                if (dis[k][j] >= Edge::inf) {
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