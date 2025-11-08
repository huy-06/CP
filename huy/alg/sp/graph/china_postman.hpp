#include "../../../ds/graph/graph.hpp"

#ifndef HUY_ALG_SP_GRAPH_CPP
#define HUY_ALG_SP_GRAPH_CPP
namespace huy {
namespace alg {

template <typename Edge>
typename Edge::value_type china_postman_problem(const ds::graph<Edge>& g) {
    using Tp = typename Edge::value_type;
    int n = g.V();
    std::vector<std::vector<Tp>> dis(n, std::vector<Tp>(n, Edge::inf));
    for (int i = 0; i < n; ++i) {
        dis[i][i] = 0;
    }
    std::vector<int> deg(n, 0);
    Tp sum_w = 0;
    for (const auto& e : g.get_edges()) {
        int u = e.from;
        int v = e.to;
        if (u < v) {
            Tp w = e.weight;
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
        if (deg[i] > 0 && dis[0][i] >= Edge::inf) {
            return Edge::inf;
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
    std::vector<Tp> dp(1 << k, Edge::inf);
    dp[0] = 0;
    for (int mask = 0; mask < (1 << k); ++mask) {
        if (dp[mask] == Edge::inf) continue;
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
} // namespace huy
#endif