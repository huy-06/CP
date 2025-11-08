#include "../../../ds/graph/graph.hpp"

#ifndef HUY_ALG_SP_GRAPH_TRAVELING_SALESMAN
#define HUY_ALG_SP_GRAPH_TRAVELING_SALESMAN
namespace huy {
namespace alg {

template<typename Edge>
std::pair<typename Edge::value_type,
          std::vector<int>> travelling_salesman_problem(const ds::graph<Edge>& g) {
    using Tp = typename Edge::value_type;
    int n = g.V();
    int full = 1 << n;
    std::vector<std::vector<Tp>>  dis(full, std::vector<Tp>(n, Edge::inf));
    std::vector<std::vector<int>> par(full, std::vector<int>(n, -1));
    dis[1][0] = 0;
    for (int mask = 1; mask < full; ++mask) {
        for (int u = 0; u < n; ++u) {
            if (!(mask & (1 << u)) || dis[mask][u] == Edge::inf) continue;
            for (const auto& e : g[u]) {
                int v = e.to;
                Tp  w = e.weight;
                if (mask & (1 << v)) continue;
                int nmask = mask | (1 << v);
                Tp  nd    = dis[mask][u] + w;
                if (dis[nmask][v] > nd) {
                    dis[nmask][v] = nd;
                    par[nmask][v] = u;
                }
            }
        }
    }
    Tp best = Edge::inf;
    int last = -1;
    for (int u = 1; u < n; ++u) {
        Tp w = Edge::inf;
        for (const auto& e : g[u]) {
            if (e.to == 0) {
                w = e.weight;
                break;
            }
        }
        if (w == Edge::inf) continue;
        if (best > dis[full - 1][u] + w) {
            best = dis[full - 1][u] + w;
            last = u;
        }
    }
    std::vector<int> path;
    if (best == Edge::inf) return { best, path };
    path.reserve(n);
    for (int mask = full - 1; last != -1;) {
        path.push_back(last);
        int p = par[mask][last];
        mask ^= 1 << last;
        last = p;
    }
    path.push_back(0);
    std::reverse(path.begin(), path.end());
    return { best, path };
}

} // namespace alg
} // namespace huy
#endif