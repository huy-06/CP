#include "../../../ds/edge/weight.hpp"
#include "../../../ds/graph/graph.hpp"

#ifndef CP_ALG_GRAPH_SHORTEST_PATH_CPP
#define CP_ALG_GRAPH_SHORTEST_PATH_CPP
namespace cp {
namespace alg {

template <typename Edge>
typename Edge::value_type china_postman_problem(const ds::graph<Edge>& g) {
    using value_type = typename Edge::value_type;
    using edge_type  = Edge;

    int n = g.num_vertices();

    std::vector<std::vector<value_type>> dis(n, std::vector<value_type>(n, edge_type::inf));
    for (int i = 0; i < n; ++i) {
        dis[i][i] = 0;
    }

    std::vector<int> deg(n, 0);
    value_type sum_w = 0;
    for (const auto& e : g.get_edges()) {
        int u = e.from;
        int v = e.to;
        if (u < v) {
            value_type w = e.weight;
            dis[u][v] = std::min(dis[u][v], w);
            dis[v][u] = dis[u][v];
            ++deg[u];
            ++deg[v];
            sum_w += w;
        }
    }

    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (dis[i][j] > dis[i][k] + dis[k][j]) {
                    dis[i][j] = dis[i][k] + dis[k][j];
                }
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        if (deg[i] > 0 && dis[0][i] >= edge_type::inf) {
            return edge_type::inf;
        }
    }

    std::vector<int> odd;
    odd.reserve(n);
    for (int i = 0; i < n; ++i) {
        if (deg[i] & 1) {
            odd.push_back(i);
        }
    }

    int k = odd.size();
    if (k == 0) {
        return sum_w;
    }

    std::vector<value_type> dp(1 << k, edge_type::inf);
    dp[0] = 0;
    for (int mask = 0; mask < (1 << k); ++mask) {
        if (dp[mask] == edge_type::inf) continue;
        int first = -1;
        for (int i = 0; i < k; ++i) {
            if (!((mask >> i) & 1)) {
                first = i;
                break;
            }
        }
        if (first == -1) continue;
        for (int j = first + 1; j < k; ++j) {
            if (!((mask >> j) & 1)) {
                int nmask = mask | (1 << first) | (1 << j);
                dp[nmask] = std::min(dp[nmask], dp[mask] + dis[odd[first]][odd[j]]);
            }
        }
    }

    return sum_w + dp[(1 << k) - 1];
}

} // namespace alg
} // namespace cp
#endif